//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <future>

#include "PMTPacket.h"
#include "TransportStream.h"

#include "FileSource.h"

std::vector<TSPacketPtr> thread_body(const TSSourcePtr & sourcePtr)
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
    mFuture = std::async(thread_body, mSourcePtr);
    // TODO: Test if std::yield can make the other thread run
}

std::vector<TSPacketPtr> TransportStream::getPackets()
{
    std::vector<TSPacketPtr> packets;
    if (!mSourcePtr->isDone())
    {
        // Aquire mutex
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
    auto start = std::begin(mPackets);
    return findPat(start);
}

TSPacketPtr TransportStream::findPat(const std::vector< TSPacketPtr >::iterator &it)
{
    if (mPackets.empty())
    {
        mPackets = getPackets();
    }
    auto pat = std::find_if(std::begin(mPackets), std::end(mPackets),
                            [](const TSPacketPtr & a) {
                            if (a->pid() == 0x00) {
                                return true;
                            }
                            return false;
                        });

    if (pat != std::end(mPackets))
    {
        return *pat;
    }
    mPackets.clear();
    return nullptr;

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

//TODO: Not used, remove or make usefull
std::vector< int > TransportStream::findPids()
{
    std::vector<int> pids;
    std::vector<int> ppids;
    TSPacketPtr pmtPacket;

    mPackets = getPackets();
    auto pat = findPat();
    auto pmtPids = findPmtPids(pat);
    pids = pmtPids;

    for (auto p : pmtPids) {
        pmtPacket = findPmt(p);
        auto pmt = parsePmt(pmtPacket);
        ppids = pmt.getElementaryPids();
        pids.insert(std::end(pids), std::begin(ppids), std::end(ppids));
    }

    return pids;
}

// TODO: Create a common find_packet(int pid, iterator it)
// That returns a packet with pid starting from it.
// Then findPmt(pid) and findPat() calls that
TSPacketPtr TransportStream::findPmt(int pid)
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

    auto pmt_pids = findPmtPids(pat);

    for (auto p : pmt_pids) {
        TSPacketPtr pmt_pkt;
        while (!pmt_pkt)
        {
            pmt_pkt = findPmt(p);
        }
        pmts.push_back(parsePmt(pmt_pkt));
    }

    return pmts;
}

void TransportStream::stop()
{
    mSourcePtr->stop();
}