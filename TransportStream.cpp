//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <algorithm>
#include <stdexcept>
#include <thread>
#include <future>

#include "PMTPacket.h"
#include "TransportStream.h"
#include "FileSource.h"

std::vector<TSPacketPtr> threadBody(const TSSourcePtr& sourcePtr)
{
    return sourcePtr->doRead();
}

TransportStream::TransportStream(TSSourcePtr sourcePtr,
                                 std::condition_variable & cond,
                                 std::mutex & mutex):
        mSourcePtr(std::move(sourcePtr)),
        mCond(cond),
        mMutex(mutex),
        mDone(false)
{
    mFuture = std::async(threadBody, mSourcePtr);
}

std::vector<TSPacketPtr> TransportStream::getPackets()
{
    std::vector<TSPacketPtr> packets;
    if (!mSourcePtr->isDone())
    {
        // Acquire mutex
        std::unique_lock< std::mutex > lock(mMutex);
        // Release mutex and wait, re-aquire mutex on wakeup
        mCond.wait(lock);
        packets = mSourcePtr->getPackets();
        // It could be done now
        mDone = mSourcePtr->isDone();
    }
    else
    {
        if (mFuture.valid())
        {
            packets = mFuture.get();
        }
        // Source read is completed
        mDone = true;
    }
    mPacketCount += packets.size();
    return packets;
}

TSPacketPtr TransportStream::findPat()
{
    return findFirstPacket(0);
}

TSPacketPtr TransportStream::findPmt(int patPid)
{
    return findFirstPacket(patPid);
}

TSPacketPtr TransportStream::findFirstPacket(int pid)
{
    if (mPackets.empty())
    {
        mPackets = getPackets();
    }
    auto it = find_if(std::begin(mPackets), std::end(mPackets),
                      [&pid](const TSPacketPtr &packet) {
                          if (packet->pid() == pid)
                          {
                              return true;
                          }
                          return false;
                      });

    if (it != std::end(mPackets))
    {
        return *it;
    }
    mPackets.clear();
    return nullptr;
}

std::vector< PMTPacket > TransportStream::getPmts(const TSPacketPtr& pat)
{
    std::vector<PMTPacket> pmts;

    auto pmtPids = findPmtPids(pat);

    for (auto p : pmtPids) {
        TSPacketPtr pmtPkt;
        while (!pmtPkt)
        {
            pmtPkt = findPmt(p);
        }
        pmts.push_back(parsePmt(pmtPkt));
    }

    return pmts;
}

std::vector< int > TransportStream::findPmtPids(const TSPacketPtr & pat) const
{
    std::vector<int> pids;
    auto programs = pat->getProgramPids();

    for (auto p : programs)
    {
        pids.push_back(p.second);
    }
    return pids;
}

void TransportStream::stop()
{
    mSourcePtr->stop();
}