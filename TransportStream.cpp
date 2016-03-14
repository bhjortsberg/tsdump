//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <fstream>
#include <sstream>
#include <stdexcept>
#include "TransportStream.h"


TransportStream::TransportStream(const std::string &fileName)
{
    std::ifstream file(fileName);
    char buffer[TSPacket::TS_PACKET_SIZE];
    if (file.is_open())
    {

        int pkt_cnt = 0;
        while (file.read(buffer, sizeof(buffer)))
        {
            if (buffer[0] == TSPacket::SYNC_BYTE)
            {
                std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE);
                std::copy(std::begin(buffer), std::end(buffer), std::begin(raw_packet));
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
