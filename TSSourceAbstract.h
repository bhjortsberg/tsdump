//
// Created by bjorn on 2019-01-14.
//

#pragma once

#include <mutex>
#include "TSSource.h"

class TSSourceAbstract : public TSSource
{
public:
    TSSourceAbstract(std::mutex& mutex);
    ~TSSourceAbstract() = default;

    std::vector<TSPacketPtr> getPackets() override;
    std::vector<TSPacketPtr> doRead() override;
    bool isDone() override;
    void stop() override;
    virtual bool isStopped() override;

protected:
    virtual std::vector<TSPacketPtr> read() = 0;
    virtual void addPacket(const std::vector< unsigned char >::const_iterator& packetStart);
    virtual void addPacket(std::vector< unsigned char > & rawPacket);
    virtual std::tuple<uint32_t, std::vector<uint8_t>> findSynchAndAddPackets(uint32_t dataSize, const std::vector<unsigned char>& rawPacket);
    virtual void addAllPacketsAndResync(uint32_t numberOfPackets, const std::vector< uint8_t > &multiPackets);

    bool mDone = false;
    bool mStop = false;
    std::map<int, TSPacketPtr> mLatestPackets;
    uint32_t mPacketCount = 0;
    std::vector<TSPacketPtr> mPackets;
    std::mutex & mMutex;

};
