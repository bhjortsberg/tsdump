//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "TransportStream.h"
#include "PMTPacket.h"


TransportStream::TransportStream(const std::string &fileName)
{
    std::ifstream file(fileName);
    std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE);
    if (file.is_open())
    {

        int pkt_cnt = 0;
        while (file.read(reinterpret_cast<char*>(raw_packet.data()), raw_packet.capacity()))
        {
            if (raw_packet[0] == TSPacket::SYNC_BYTE)
            {

                add_packet(raw_packet, pkt_cnt);
                pkt_cnt++;
            }
            else
            {
                // TODO: Out of sync
                throw std::runtime_error("Error in sync byte");
            }
        }

    }
    else
    {
        std::stringstream estr;
        estr << "Failed to open file: " <<  fileName;
        throw std::runtime_error(estr.str());
    }
}

std::vector<TSPacketPtr> TransportStream::getPackets()
{
    return packets;
}

void TransportStream::add_packet(std::vector< unsigned char > raw_packet, int cnt)
{

    TSPacketPtr packet(new TSPacket(raw_packet, cnt));
    auto p = m_latest_packets.find(packet->pid());
    if (p != std::end(m_latest_packets))
    {
        p->second->set_next(packet);
        packet->set_prev(p->second);
        m_latest_packets.erase(p);
    }
    m_latest_packets.emplace(packet->pid(), packet);

    packets.push_back(packet);

}

std::vector< TSPacketPtr >::iterator TransportStream::find_pat()
{
    auto start = std::begin(packets);
    return find_pat(start);
}

// TODO: Use TSPacket instead of the iterator?
std::vector< TSPacketPtr >::iterator TransportStream::find_pat(const std::vector< TSPacketPtr >::iterator &it)
{
    auto start = it;
    return std::find_if(start, std::end(packets),
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
    auto it = find_if(std::begin(packets), std::end(packets),
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


