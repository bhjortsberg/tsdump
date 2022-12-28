//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <stdexcept>
#include <iostream>
#include "TSPacket.h"

TSPacket::TSPacket(Chunk buffer, int pkt_num):
        mChunk(buffer),
        mPacketNumber(pkt_num),
        mPrev(nullptr),
        mNext(nullptr)
{
    mSyncByte = mChunk[0];
    mTransportErrorIndicator      = (mChunk[1] & 0x80) != 0;
    mPayloadUnitStartIndicator    = (mChunk[1] & 0x40) != 0;
    mTransportPriority            = (mChunk[1] & 0x20) != 0;
    mContinuityCount              = mChunk[3] & 0xf;

    // Initialize payload, pes and adaptation field iterators
    mPayloadIt = std::begin(mChunk) + 4;
    mPesHeaderIt = std::begin(mChunk) + 4;
    mAdaptationFieldIt = std::begin(mChunk) + 4;

    if (hasAdaptationField()) {
        mPesHeaderIt = mAdaptationFieldIt + adaptationField().size();
        mPayloadIt = mPesHeaderIt;
    }

    if (hasPesHeader()) {
        mPayloadIt += pesHeader().getLength();
    }
}

unsigned short TSPacket::pid() const
{
    int pid = (mChunk[1] & 0x1f) << 8;
    pid |= mChunk[2];
    return pid;
}

unsigned short TSPacket::continuityCount() const
{
    return mContinuityCount;
}

bool TSPacket::hasAdaptationField() const
{
    return (adaptationFieldControl() & 0x02) != 0;
}

char TSPacket::adaptationFieldControl() const
{
    return (mChunk[3] & 0x30) >> 4;
}

bool TSPacket::hasRandomAccessIndicator() const
{
    return hasAdaptationField() ? adaptationField().hasRandomAccessIndicator() : false;
}

AdaptationField TSPacket::adaptationField() const
{
    if (hasAdaptationField())
    {
        return AdaptationField(mAdaptationFieldIt);
    }

    throw std::runtime_error("No adaptation field");
}

PESHeader TSPacket::pesHeader() const
{
    if (hasPesHeader())
    {
        return PESHeader(mPesHeaderIt);
    }

    throw std::runtime_error("No PES Header");
}

bool TSPacket::hasPesHeader() const
{
    if ( *mPesHeaderIt == 0x00 &&
         *(mPesHeaderIt + 1) == 0x00 &&
         *(mPesHeaderIt + 2) == 0x01) {
        return true;
    }

    return false;
}

bool TSPacket::hasEbp() const
{
    return hasAdaptationField() && adaptationField().hasEbp();
}

PayloadIterator TSPacket::getPayload() const {
    return {mPayloadIt, std::end(mChunk)};
}

bool TSPacket::isPayloadStart() const
{
    return mPayloadUnitStartIndicator;
}

int TSPacket::number() const
{
    return mPacketNumber;
}

void TSPacket::setNextPacket(const TSPacketPtr & next)
{
    mNext = next;
}

void TSPacket::setPreviousPacket(const TSPacketPtr & prev)
{
    mPrev = prev;
}

bool TSPacket::continuity() const
{
    if (!mPrev)
        return true;

    if ((adaptationFieldControl() & 0x1) == 0) {
        // No payload flag is set - continuity is not incremented
        // Packet has only adaptation field
        return true;
    }

    if (pid() == 8191) {
        // This is a null packet for bandwidth padding - continuity is not incremented
        return true;
    }
    return (mPrev->continuityCount() + 1) % 16 == mContinuityCount;
}

TSPacketPtr TSPacket::getPreviousPacket() const
{
    return mPrev;
}

std::map< unsigned short, unsigned short> TSPacket::getProgramPids() const
{
    if (pid() != 0x00) {
        throw std::runtime_error("Not a PAT");
    }

    auto pit = mPayloadIt + 9;
    // byte 9 is lastSectionNumber
    // byte 10-11 program number
    // byte 12-13 program_map_pid - 13 bits
    auto sectionNumber = *(mPayloadIt + 7);
    auto lastSectionNumber =  *(mPayloadIt + 8);

    std::map<unsigned short, unsigned short> result;

    while (sectionNumber++ <= lastSectionNumber)
    {
        unsigned short programNumber = 0;
        unsigned short programMapPid = 0;

        programNumber |= *(pit++) << 8;
        programNumber |= *(pit++);
        if (programNumber != 0)
        {
            programMapPid |= (*(pit++) & 0x1F) << 8;
            programMapPid |= *(pit++);
            result.insert( {programNumber, programMapPid} );
        }
    }

    return result;
}

const Chunk::const_iterator& TSPacket::payload() const
{
    return mPayloadIt;
}

const uint8_t * TSPacket::raw() const
{
    return mChunk.data();
}

size_t TSPacket::size() const
{
    return mChunk.size();
}

const TSPacket* TSPacket::findPrevPesPacket()
{
    TSPacket* currentPacket = this;

    while ((currentPacket = currentPacket->getPreviousPacket().get()) != nullptr)
    {
        if (currentPacket->hasPesHeader())
        {
            break;
        }
    }
    return currentPacket;
}

uint32_t findSynchByte(const std::vector<uint8_t>::const_iterator& sourcePackets, uint32_t size)
{
    uint32_t synchByte = 0;
    while (*(sourcePackets + synchByte) != TSPacket::SYNC_BYTE ||
           *(sourcePackets + synchByte + TSPacket::TS_PACKET_SIZE) != TSPacket::SYNC_BYTE)
    {
        if (++synchByte + TSPacket::TS_PACKET_SIZE > size)
        {
            throw std::runtime_error("Cannot find sync byte");
        }
    }
    return synchByte;
}