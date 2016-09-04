//
// Created by Björn Hjortsberg on 28/06/16.
//

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include "FileSource.h"

FileSource::FileSource(const std::string &source,
                       std::condition_variable & cond,
                       std::mutex & mutex,
                       std::vector<TSPacketPtr> & packets) :
m_filename(source),
m_packets(packets),
m_partially_read(cond),
m_mutex(mutex)
{

}


std::vector<TSPacketPtr> FileSource::operator()()
{
    std::ifstream file(m_filename);
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
                if (pkt_cnt % 10000 == 0)
                {
                    // Notify that packets has been read
                    m_partially_read.notify_one();
                }
            }
            else
            {
                // TODO: Out of sync
                throw std::runtime_error("Error in sync byte");
            }
        }
        // Notify that packets has been read
        m_partially_read.notify_one();

    }
    else
    {
        std::stringstream estr;
        estr << "Failed to open file: " <<  m_filename;
        throw std::runtime_error(estr.str());
    }

    return m_packets;

}

void FileSource::add_packet(std::vector< unsigned char > & raw_packet, int cnt)
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

    std::unique_lock<std::mutex> lock(m_mutex);
    m_packets.push_back(packet);

}

std::vector< TSPacketPtr > FileSource::getPackets() const
{
    return m_packets;
}




