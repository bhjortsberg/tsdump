//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "TSPacket.h"
#include "PMTPacket.h"

TSPacket::TSPacket(Chunk buffer, int pkt_num):
        mChunk(buffer),
        mPacketNumber(pkt_num),
        mPrev(nullptr),
        mNext(nullptr)
{
    mSyncByte = mChunk[0];
    mTransportErrorIndicator       = mChunk[1] & 0x80 ? true : false;
    mPayloadUnitStartIndicator    = mChunk[1] & 0x40 ? true : false;
    mTransportPriority              = mChunk[1] & 0x20 ? true : false;
    mContinuityCount                = mChunk[3] & 0xf;

    // Initialize payload, pes and adaptation field iterators
    payloadIt = std::begin(mChunk) + 4;
    pesHeaderIt = std::begin(mChunk) + 4;
    adaptationFieldIt = std::begin(mChunk) + 4;

    if (hasAdaptationField()) {
        pesHeaderIt = adaptationFieldIt + adaptationField().size();
        payloadIt = pesHeaderIt;
    }

    if (hasPesHeader()) {
        payloadIt += pesHeader().getLength();
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
    return hasAdaptationField() ? adaptationField().has_random_access_indicator() : false;
}

AdaptationField TSPacket::adaptationField() const
{
    if (hasAdaptationField())
    {
        return AdaptationField(std::begin(mChunk) + 4);
//        return AdaptationField(adaptationFieldIt);
    }

    throw std::runtime_error("No adaptation field");

}

PESHeader TSPacket::pesHeader() const
{
    if (hasPesHeader())
    {
        int pes_pos = 4;

        if (hasAdaptationField())
        {
            pes_pos += adaptationField().size();
        }

        return PESHeader(std::begin(mChunk) + pes_pos);
    }

    throw std::runtime_error("No PES Header");
}

bool TSPacket::hasPesHeader() const
{
    int pes_pos = 4;
    if (hasAdaptationField()) {
        pes_pos += adaptationField().size();
    }

    if (mChunk[pes_pos] == 0x00 &&
        mChunk[pes_pos + 1] == 0x00 &&
        mChunk[pes_pos + 2] == 0x01) {
        return true;
    }

    return false;
}

bool TSPacket::hasEbp() const
{
    return hasAdaptationField() && adaptationField().has_ebp();
}

Chunk TSPacket::getPayload() const {
    Chunk payload(TS_PACKET_SIZE);
//    std::copy(payloadIt, std::end(mChunk), std::begin(payload));
    std::copy(std::begin(mChunk), std::end(mChunk), std::begin(payload));
    return payload;
//    return PayloadIterator(payloadIt, std::end(mChunk));
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

    auto pit = payloadIt + 9;
    // byte 9 is last_section_number
    // byte 10-11 program number
    // byte 12-13 program_map_pid - 13 bits
    auto section_number = *(payloadIt + 7);
    auto last_section_number =  *(payloadIt + 8);

    std::map<unsigned short, unsigned short> result;

    // TODO: Replace with something better
    for (int i = section_number; i <= last_section_number; ++i)
    {
        unsigned short program_number = 0;
        unsigned short program_map_pid = 0;

        program_number |= *(pit++) << 8;
        program_number |= *(pit++);
        if (program_number != 0)
        {
            program_map_pid |= (*(pit++) & 0x1F) << 8;
            program_map_pid |= *(pit++);
            result.insert( {program_number, program_map_pid} );
        }
    }

    return result;
}

Chunk::const_iterator TSPacket::payload() const
{
    return payloadIt;
}

const uint8_t * TSPacket::raw() const
{
    return mChunk.data();
}

size_t TSPacket::size() const
{
    return mChunk.size();
}

uint32_t findSynchByte(const std::vector<uint8_t>::const_iterator& src_packets, uint32_t size)
{
    uint32_t synch_byte = 0;
    while (*(src_packets + synch_byte) != TSPacket::SYNC_BYTE ||
           *(src_packets + synch_byte + TSPacket::TS_PACKET_SIZE) != TSPacket::SYNC_BYTE)
    {
        if (++synch_byte + TSPacket::TS_PACKET_SIZE > size)
        {
            throw std::runtime_error("Cannot find sync byte");
        }
    }
    return synch_byte;
}

