//
// Created by bjorn on 2019-01-14.
//

#include "TSSourceAbstract.h"

TSSourceAbstract::TSSourceAbstract(std::mutex &mutex):
mMutex(mutex)
{

}

void TSSourceAbstract::add_packet(const std::vector< unsigned char >::const_iterator& packet_start)
{
    std::vector<unsigned char> packet(packet_start, packet_start + TSPacket::TS_PACKET_SIZE);
    add_packet(packet);
}

void TSSourceAbstract::add_packet(std::vector< unsigned char > & raw_packet)
{

    TSPacketPtr packet(new TSPacket(raw_packet, mPacketCount));
    auto p = mLatestPackets.find(packet->pid());
    if (p != std::end(mLatestPackets))
    {
        p->second->set_next(packet);
        packet->set_prev(p->second);
        mLatestPackets.erase(p);
    }
    mLatestPackets.emplace(packet->pid(), packet);

    {
        std::lock_guard< std::mutex > lock(mMutex);
        mPackets.push_back(packet);
    }
    ++mPacketCount;
}

std::tuple<uint32_t, std::vector<uint8_t>> TSSourceAbstract::findSynchAndAddPackets(uint32_t dataSize, const std::vector<unsigned char>& raw_packet)
{
    if (dataSize == 0) {
        return std::make_tuple(0, std::vector<uint8_t>());
    }
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

void TSSourceAbstract::addAllPacketsAndResync(uint32_t numberOfPackets, const std::vector< uint8_t > &multi_packets){
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

std::vector< TSPacketPtr > TSSourceAbstract::getPackets()
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
    return std::move(mPackets);
}

std::vector<TSPacketPtr> TSSourceAbstract::doRead()
{
    return read();
}

bool TSSourceAbstract::isDone()
{
    return mDone;
}

void TSSourceAbstract::stop()
{
    mStop = true;
}

