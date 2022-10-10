//
// Created by Björn Hjortsberg on 2016-02-20.
//

#pragma once

#include <condition_variable>
#include <future>
#include <iosfwd>
#include <vector>
#include <map>
#include <mutex>
#include "TSPacket.h"
#include "TSSource.h"
#include "PMTPacket.h"

class TransportStream
{
public:
    TransportStream(TSSourcePtr sourcePtr,
                    std::condition_variable & cond,
                    std::mutex & mutex);

    std::vector<TSPacketPtr> getPackets();
    TSPacketPtr findPat();
    TSPacketPtr findPat(const std::vector<TSPacketPtr>::iterator & it );
    std::vector<int> findPmtPids(const TSPacketPtr & pat) const;
    std::vector<int> findPids();
    TSPacketPtr findPmt(int pid);
    std::vector<PMTPacket> getPmts(const TSPacketPtr& pat);
    bool isDone() const { return mDone; }
    bool isStopped() const { return mSourcePtr->isStopped(); }
    void stop();

private:
    std::vector<TSPacketPtr> mPackets;
    std::vector<TSPacketPtr>::iterator mCurrentPacket;
    std::future<std::vector<TSPacketPtr>> mFuture;
    TSSourcePtr mSourcePtr;
    std::condition_variable & mCond;
    std::mutex & mMutex;
    bool mDone;
    uint32_t mPacketCount = 0;
};


