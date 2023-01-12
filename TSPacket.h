//
// Created by Björn Hjortsberg on 2016-02-20.
//

#pragma once

#include <vector>
#include <map>
#include <memory>
#include "AdaptationField.h"
#include "PESHeader.h"
#include "IPacket.h"


uint32_t findSynchByte(const std::vector<uint8_t>::const_iterator& sourcePackets, uint32_t size);

class PacketIterator
{
public:
    PacketIterator(Chunk::const_iterator start, Chunk::const_iterator end) :
    mStart(start),
    mEnd(end)
    {}
    const Chunk::const_iterator& begin()
    {
        return mStart;
    }
    const Chunk::const_iterator& end()
    {
        return mEnd;
    }
private:
    Chunk::const_iterator mStart;
    Chunk::const_iterator mEnd;
};

class TSPacket;
using TSPacketPtr = std::shared_ptr<TSPacket>;

class TSPacket : public IPacket
{
public:
    static const int SYNC_BYTE = 0x47;
    static const int TS_PACKET_SIZE = 188;

    TSPacket(Chunk buffer, int pkt_num);
    unsigned short pid() const;
    unsigned short continuityCount() const;
    bool hasAdaptationField() const;
    bool hasEbp() const;
    bool hasRandomAccessIndicator() const;
    AdaptationField adaptationField() const;
    bool hasPesHeader() const;
    PESHeader pesHeader() const;
    PacketIterator getBytes() const;
    bool isPayloadStart() const;
    int number() const;
    void setNextPacket(const TSPacketPtr & next);
    void setPreviousPacket(const TSPacketPtr & prev);
    bool continuity() const;
    TSPacketPtr getPreviousPacket() const;
    std::map<unsigned short, unsigned short> getProgramPids() const;
    std::vector<int> parsePmt() const;

    const Chunk::const_iterator& payload() const override;
    const uint8_t * raw() const;
    size_t size() const;
    const TSPacket* findPrevPesPacket();

private:
    char adaptationFieldControl() const;
    Chunk mChunk;
    char mSyncByte;
    bool mTransportErrorIndicator;
    bool mPayloadUnitStartIndicator;
    bool mTransportPriority;
    unsigned short mContinuityCount;
    int mPacketNumber;

    TSPacketPtr mPrev;
    TSPacketPtr mNext;

    Chunk::iterator mPesHeaderIt;
    Chunk::iterator mAdaptationFieldIt;
    Chunk::const_iterator mPayloadIt;
};