//
// Created by bjorn on 2019-01-14.
//

#include "TSSourceAbstract.h"

TSSourceAbstract::TSSourceAbstract(std::mutex &mutex):
mMutex(mutex)
{

}

void TSSourceAbstract::addPacket(const std::vector< unsigned char >::const_iterator& packetStart)
{
    std::vector<unsigned char> packet(packetStart, packetStart + TSPacket::TS_PACKET_SIZE);
    addPacket(packet);
}

void TSSourceAbstract::addPacket(std::vector< unsigned char > & rawPacket)
{

    TSPacketPtr packet(new TSPacket(rawPacket, mPacketCount));
    auto p = mLatestPackets.find(packet->pid());
    if (p != std::end(mLatestPackets))
    {
        p->second->setNextPacket(packet);
        packet->setPreviousPacket(p->second);
        mLatestPackets.erase(p);
    }
    mLatestPackets.emplace(packet->pid(), packet);

    {
        std::lock_guard< std::mutex > lock(mMutex);
        mPackets.push_back(packet);
    }
    ++mPacketCount;
}

std::tuple<uint32_t, std::vector<uint8_t>> TSSourceAbstract::findSynchAndAddPackets(uint32_t dataSize, const std::vector<unsigned char>& rawPacket)
{
    if (dataSize == 0) {
        return std::make_tuple(0, std::vector<uint8_t>());
    }
    uint32_t syncByte = findSynchByte(rawPacket.begin(), dataSize);
    // Sync byte found, add packets to list
    while (syncByte + (mPacketCount + 1) * TSPacket::TS_PACKET_SIZE < dataSize + 1)
    {
        addPacket(rawPacket.begin() + syncByte + mPacketCount * TSPacket::TS_PACKET_SIZE);
    }

    uint32_t numPackets = 100;
    uint32_t bytesLeft = dataSize - (syncByte + mPacketCount * TSPacket::TS_PACKET_SIZE);

    std::vector<uint8_t> multiPackets(numPackets * TSPacket::TS_PACKET_SIZE + bytesLeft);
    // Copy part of last packet and then read rest of packet and add to list
    std::copy(rawPacket.begin() + dataSize - bytesLeft, rawPacket.begin() + dataSize, multiPackets.begin());
    return std::make_tuple(bytesLeft, multiPackets);
}

void TSSourceAbstract::addAllPacketsAndResync(uint32_t numberOfPackets, const std::vector< uint8_t > &multiPackets){
    // Add all received packets to packet list
    uint32_t i = 0;
    uint32_t syncByte = 0;
    while (i < numberOfPackets)
    {
        if (multiPackets[i * TSPacket::TS_PACKET_SIZE + syncByte] == TSPacket::SYNC_BYTE)
        {
            addPacket(multiPackets.begin() + syncByte + (i * TSPacket::TS_PACKET_SIZE));
            i++;
        }
        else
        {
            syncByte = findSynchByte(
                    multiPackets.begin() + i * TSPacket::TS_PACKET_SIZE,
                    (numberOfPackets - i) * TSPacket::TS_PACKET_SIZE);
        }
    }
}

std::vector< TSPacketPtr > TSSourceAbstract::getPackets()
{
    //std::unique_lock< std::mutex > lock(mMutex);
    //m_partially_read.wait(lock);
/*    auto packets = mPackets;
    {
        std::cout << "Get packets and clear list\n";
        //std::lock_guard< std::mutex > lock(mMutex);
        mPackets.clear();
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

bool TSSourceAbstract::isStopped()
{
    return mStop;
}