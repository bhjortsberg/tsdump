//
// Created by Björn Hjortsberg on 28/06/16.
//

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <future>
#include "FileSource.h"


FileSource::FileSource(const std::string &source,
                       std::condition_variable & cond,
                       std::mutex & mutex) :
m_filename(source),
m_partially_read(cond),
m_mutex(mutex)
{
    m_lastRetreivedPacket = m_nullIterator;
    std::ifstream file(m_filename, std::fstream::in);
    if (!file.good())
    {
        throw std::runtime_error("Failed to open file: " + m_filename);
    }
}

std::vector<TSPacketPtr> FileSource::read()
{
    std::ifstream file(m_filename, std::fstream::in);

    std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE * 10);
    file.seekg(0, std::istream::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::istream::beg);

    if (file.is_open())
    {
        file.read(reinterpret_cast<char*>(raw_packet.data()), raw_packet.capacity());

        auto [bytes_left, multi_packets] = findSynchAndAddPackets(raw_packet.size(), raw_packet);

        while (not m_stop)
        {
            uint32_t pos = bytes_left;
            bytes_left = 0;
            uint64_t bytes_left_in_file = fileSize - file.tellg();
            uint64_t readBytes = multi_packets.capacity() - pos;
            if (readBytes > bytes_left_in_file)
            {
                readBytes = bytes_left_in_file;
            }
            file.read(reinterpret_cast<char*>(multi_packets.data() + pos),
                      readBytes);
            pos += readBytes;

            if (file.tellg() < 0 || file.tellg() == fileSize)
            {
                m_stop = true;
            }

            auto numberOfPackets = pos/TSPacket::TS_PACKET_SIZE;
            addAllPacketsAndResync(numberOfPackets, multi_packets);

            // Notify that packets has been read
            m_partially_read.notify_one();
        }
        m_done = true;
        //File read done notify the remaining packets
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

void FileSource::add_packet(const std::vector< unsigned char >::const_iterator& packet_start)
{
    std::vector<unsigned char> packet(packet_start, packet_start + TSPacket::TS_PACKET_SIZE);
    add_packet(packet);
}

void FileSource::add_packet(std::vector< unsigned char > & raw_packet)
{

    TSPacketPtr packet(new TSPacket(raw_packet, mPacketCount));
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
    ++mPacketCount;
}

std::tuple<uint32_t, std::vector<uint8_t>> FileSource::findSynchAndAddPackets(uint32_t dataSize, const std::vector<unsigned char>& raw_packet)
{
    uint32_t sync_byte = find_synch_byte(raw_packet.begin(), dataSize);
    // Sync byte found, add packets to list
    while (sync_byte + (mPacketCount + 1) * TSPacket::TS_PACKET_SIZE < dataSize + 1)
    {
        add_packet(raw_packet.begin() + sync_byte + mPacketCount * TSPacket::TS_PACKET_SIZE);
    }

    uint32_t num_packets = 100;
    uint32_t bytes_left = dataSize - (sync_byte + mPacketCount * TSPacket::TS_PACKET_SIZE);

    std::vector<uint8_t> multi_packets(num_packets * TSPacket::TS_PACKET_SIZE + bytes_left);
    // Copy part of last packet and then read rest of packet and add to list
    std::copy(raw_packet.begin() + dataSize - bytes_left, raw_packet.begin() + dataSize, multi_packets.begin());
    return std::make_tuple(bytes_left, multi_packets);
}

void FileSource::addAllPacketsAndResync(uint32_t numberOfPackets, const std::vector< uint8_t > &multi_packets){
    // Add all received packets to packet list
    uint32_t i = 0;
    uint32_t sync_byte = 0;
    while (i < numberOfPackets)
    {
        if (multi_packets[i * TSPacket::TS_PACKET_SIZE + sync_byte] == TSPacket::SYNC_BYTE)
        {
            add_packet(multi_packets.begin() + sync_byte + (i*TSPacket::TS_PACKET_SIZE));
            i++;
        }
        else
        {
            sync_byte = find_synch_byte(
                    multi_packets.begin() + i * TSPacket::TS_PACKET_SIZE,
                    (numberOfPackets - i) * TSPacket::TS_PACKET_SIZE);
        }
    }
}

std::vector< TSPacketPtr > FileSource::getPackets()
{
    //std::unique_lock< std::mutex > lock(m_mutex);
    //m_partially_read.wait(lock);
/*    auto packets = m_packets;
    {
        std::cout << "Get packets and clear list\n";
        //std::lock_guard< std::mutex > lock(m_mutex);
        m_packets.clear();
    }
    return packets;*/
    return std::move(m_packets);
}

std::vector<TSPacketPtr> FileSource::doRead()
{
    return read();
}

bool FileSource::isDone()
{
    return m_done;
}

void FileSource::stop()
{
    m_stop = true;
}

