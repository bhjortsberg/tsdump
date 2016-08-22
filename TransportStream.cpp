//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <thread>

#include "PMTPacket.h"
#include "TransportStream.h"


TransportStream::TransportStream(const TSSourcePtr &sourcePtr,
                                 std::condition_variable & cond,
                                 std::mutex & mutex):
m_cond(cond),
m_sourcePtr(sourcePtr),
m_mutex(mutex),
m_done(false)
{
    sourcePtr->async();
}

std::vector<TSPacketPtr> TransportStream::getPackets()
{
    // Aquire mutex
    std::unique_lock<std::mutex> lock(m_mutex);
    // Release mutex and wait, re-aquire mutex on wakeup
    if (m_cond.wait_for(lock, std::chrono::milliseconds(200)) == std::cv_status::timeout)
    {
        m_done = true;
    }

    return m_sourcePtr->getNewPackets();
}


std::vector< TSPacketPtr >::iterator TransportStream::find_pat()
{
    auto start = std::begin(m_packets);
    return find_pat(start);
}

// TODO: Use TSPacket instead of the iterator?
std::vector< TSPacketPtr >::iterator TransportStream::find_pat(const std::vector< TSPacketPtr >::iterator &it)
{
    auto start = it;
    return std::find_if(start, std::end(m_packets),
                        [](const TSPacketPtr & a) {
                            if (a->pid() == 0x00) {
                                return true;
                            }
                            return false;
                        });

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

std::vector< int > TransportStream::find_pids()
{
    std::vector<int> pids;
    std::vector<int> ppids;
    TSPacketPtr pmt_pkt;

    auto pat = find_pat();
    auto pmt_pids = find_pmt_pids(*pat);
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

    return *it;
}

std::vector< PMTPacket > TransportStream::get_pmts()
{
    std::vector<PMTPacket> pmts;
    TSPacketPtr pmt_pkt;

    auto pat = find_pat();
    auto pmt_pids = find_pmt_pids(*pat);

    for (auto p : pmt_pids) {
        pmt_pkt = find_pmt(p);
        pmts.push_back(parse_pmt(pmt_pkt));
    }

    return pmts;
}


