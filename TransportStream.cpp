//
// Created by Björn Hjortsberg on 20/02/16.
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
m_sourcePtr(std::move(sourcePtr)),
m_cond(cond),
m_mutex(mutex),
m_done(false)
{
    m_future = std::async(thread_body, m_sourcePtr);
    // TODO: Test if std::yield can make the other thread run
}

std::vector<TSPacketPtr> TransportStream::getPackets()
{
    std::vector<TSPacketPtr> packets;
    if (!m_sourcePtr->isDone())
    {
        // Aquire mutex
        std::unique_lock< std::mutex > lock(m_mutex);
        // Release mutex and wait, re-aquire mutex on wakeup
        m_cond.wait(lock);
        packets = m_sourcePtr->getPackets();
        // It could be done now
        m_done = m_sourcePtr->isDone();
    }
    else
    {
        packets = m_future.get();
        // Source read is completed
        m_done = true;
    }
    m_packetCount += packets.size();
    return packets;
}

TSPacketPtr TransportStream::find_pat()
{
    auto start = std::begin(m_packets);
    return find_pat(start);
}

TSPacketPtr TransportStream::find_pat(const std::vector< TSPacketPtr >::iterator &it)
{

    if (m_packets.empty())
    {
        m_packets = getPackets();
    }
    auto pat = std::find_if(std::begin(m_packets), std::end(m_packets),
                        [](const TSPacketPtr & a) {
                            if (a->pid() == 0x00) {
                                return true;
                            }
                            return false;
                        });

    if (pat != std::end(m_packets))
    {
        return *pat;
    }
    std::cout << "Could not find PAT\n";
    return nullptr;

}

std::vector< int > TransportStream::find_pmt_pids(const TSPacketPtr & pat) const
{
    std::vector<int> pids;
    auto programs = pat->get_program_pids();

    for (auto p : programs)
    {
        pids.push_back(p.second);
    }
    return pids;
}

//TODO: Not used, remove of make usefull
std::vector< int > TransportStream::find_pids()
{
    std::vector<int> pids;
    std::vector<int> ppids;
    TSPacketPtr pmt_pkt;

    m_packets = getPackets();
    auto pat = find_pat();
    auto pmt_pids = find_pmt_pids(pat);
    pids = pmt_pids;

    for (auto p : pmt_pids) {
        pmt_pkt = find_pmt(p);
        auto pmt = parse_pmt(pmt_pkt);
        ppids = pmt.get_elementary_pids();
        pids.insert(std::end(pids), std::begin(ppids), std::end(ppids));
    }

    return pids;
}

// TODO: Create a common find_packet(int pid, iterator it)
// That returns a packet with pid starting from it.
// Then find_pmt(pid) and find_pat() calls that
TSPacketPtr TransportStream::find_pmt(int pid)
{
    auto it = find_if(std::begin(m_packets), std::end(m_packets),
            [&pid] (const TSPacketPtr& packet) {
                if (packet->pid() == pid) {
                    return true;
                }
                return false;
            });

    if (it != std::end(m_packets))
    {
        return *it;
    }
    std::cout << "Could not packet with pid: " << pid << "\n";
    return nullptr;
}

std::vector< PMTPacket > TransportStream::get_pmts()
{
    std::vector<PMTPacket> pmts;
    TSPacketPtr pmt_pkt;

    auto pat = find_pat();
    auto pmt_pids = find_pmt_pids(pat);

    for (auto p : pmt_pids) {
        pmt_pkt = find_pmt(p);
        pmts.push_back(parse_pmt(pmt_pkt));
    }

    return pmts;
}


