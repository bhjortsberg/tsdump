//
// Created by bjorn on 2017-11-21.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include "MulticastSource.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

MulticastSource::MulticastSource(const std::string &source, std::condition_variable &cond, std::mutex &mutex)
:m_partially_read(cond),
 m_mutex(mutex),
 m_joined(false),
 m_done(false)
{
    size_t port_delim = source.find(':');
    if (port_delim != std::string::npos)
    {
        m_addr = source.substr(0, port_delim);
        auto port_str = source.substr(port_delim + 1, source.size() - port_delim -1);
        uint16_t port = atoi(port_str.c_str());
        m_port = htons(port);
        m_sock = socket(AF_INET, SOCK_DGRAM, 0);
        join(m_addr);
    } else {
        throw std::runtime_error("ERROR: Invalid address format should be <ip>:<port>");
    }
}

void MulticastSource::join(const std::string & addr)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(addr.c_str());
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (!setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        m_joined = true;
    }

    // Set non blocking socket
    uint32_t nonblock = 1;
    ioctl(m_sock, FIONBIO, &nonblock);

}

std::vector<TSPacketPtr> MulticastSource::getPackets() {
    auto packets = m_packets;
    m_packets.clear();
    return packets;
}

std::vector<TSPacketPtr> MulticastSource::doRead() {
    struct sockaddr_in addr;
    addr.sin_port = m_port;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_sock, (struct sockaddr*)&addr, sizeof(addr)))
    {
        perror("bind error");
        throw std::runtime_error("Bind error: " + std::string(strerror(errno)));
    }

    std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE * 2);
    socklen_t len;
    ssize_t bytes;

    int pkt_cnt = 0;
    uint32_t i = 0;
    bytes = recvfrom(m_sock, raw_packet.data(), raw_packet.capacity(), 0, (struct sockaddr*)&addr, &len);
    while (raw_packet[i] != TSPacket::SYNC_BYTE && raw_packet[i+TSPacket::TS_PACKET_SIZE])
    {
        if (++i > raw_packet.size())
        {
            throw std::runtime_error("Cannot find sync byte");
        }
    }

    // Sync byte found, copy first packet and add to list
    std::vector<uint8_t> packet(TSPacket::TS_PACKET_SIZE);
    std::copy(raw_packet.begin() + i, raw_packet.begin() + i + TSPacket::TS_PACKET_SIZE, packet.begin());
    add_packet(packet, pkt_cnt);
    pkt_cnt++;

    // Copy part of next packet and then read rest of packet and add to list
    std::copy(raw_packet.begin() + i + TSPacket::TS_PACKET_SIZE, raw_packet.end(), packet.begin());
    recvfrom(m_sock, raw_packet.data(), TSPacket::TS_PACKET_SIZE - i, 0, (struct sockaddr*)&addr, &len);
    std::copy(raw_packet.begin(), raw_packet.begin() + TSPacket::TS_PACKET_SIZE - i, packet.begin() + i);
    add_packet(packet, pkt_cnt);
    pkt_cnt++;

    uint32_t total = 0;
    uint32_t num_packets = 100;
    std::vector<uint8_t> multi_packets(num_packets*TSPacket::TS_PACKET_SIZE);

    while (not m_stop)
    {
        // TODO: Add multiplexing
        uint32_t pos = 0;
        // Fill destination vector with packets
        while ((bytes = recvfrom(m_sock, multi_packets.data() + pos,
                                 multi_packets.capacity() - pos,
                                 0,
                                 (struct sockaddr*)&addr,
                                 &len)))
        {
            if (bytes < 0 && errno == EAGAIN)
            {
                usleep(10000);
                continue;
            }
            pos += bytes;
            if (pos == multi_packets.size())
            {
                // TODO: This will miss some packets
                // Destination buffer is full
                break;
            }
            if (bytes < 0 && errno != EAGAIN)
            {
                perror("recvfrom");
                // TODO: Handle exception in thread
                throw std::runtime_error(std::string(strerror(errno)));
            }
        }
        if (bytes == 0)
        {
            std::cout << "Read 0 bytes, pos: " << pos << "\n";
        }

        // Loop all packets
        uint32_t i = 0;
        for (; i < num_packets; i++)
        {
            if (multi_packets[i * TSPacket::TS_PACKET_SIZE] == TSPacket::SYNC_BYTE)
            {
                add_packet(multi_packets.begin() + (i*TSPacket::TS_PACKET_SIZE), pkt_cnt);
                pkt_cnt++;
            }
            else
            {
                std::cout << "Synch error\n";
                // TODO: Out of sync
                // TODO: Handle throw in async task
                throw std::runtime_error("Error in sync byte");
            }
        }
        // Notify that packets has been read
        m_partially_read.notify_one();
    }

    m_done = true;
    std::vector<TSPacketPtr> packets;
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        packets = m_packets;
    }
    return packets;
}

void MulticastSource::add_packet(std::vector< unsigned char > & raw_packet, int cnt)
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

    {
        std::lock_guard< std::mutex > lock(m_mutex);
        m_packets.push_back(packet);
    }
}
void MulticastSource::add_packet(const std::vector< unsigned char >::iterator& packet_start, int cnt)
{
    std::vector<unsigned char> packet(packet_start, packet_start + TSPacket::TS_PACKET_SIZE);
    add_packet(packet, cnt);
}

bool MulticastSource::isDone() {
    return m_done;
}

void MulticastSource::stop()
{
    m_stop = true;
}