//
// Created by Björn Hjortsberg on 2016-06-28.
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
TSSourceAbstract(mutex),
mFilename(source),
mPartiallyRead(cond)
{
    std::ifstream file(mFilename, std::fstream::in);
    if (!file.good())
    {
        throw std::runtime_error("Failed to open file: " + mFilename);
    }
}

std::vector<TSPacketPtr> FileSource::read()
{
    std::ifstream file(mFilename, std::fstream::in);

    std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE * 10);
    file.seekg(0, std::istream::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::istream::beg);

    if (file.is_open())
    {
        file.read(reinterpret_cast<char*>(raw_packet.data()), raw_packet.capacity());

        auto [bytes_left, multi_packets] = findSynchAndAddPackets(raw_packet.size(), raw_packet);

        while (not mStop)
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

            auto numberOfPackets = pos/TSPacket::TS_PACKET_SIZE;
            addAllPacketsAndResync(numberOfPackets, multi_packets);

            if (file.tellg() < 0 || file.tellg() == fileSize)
            {
                break;
            }

            // Notify that packets has been read
            mPartiallyRead.notify_one();
        }
        mDone = true;
        //File read done notify the remaining packets
        mPartiallyRead.notify_one();

    }
    else
    {
        std::stringstream estr;
        estr << "Failed to open file: " <<  mFilename;
        throw std::runtime_error(estr.str());
    }
    return mPackets;
}

