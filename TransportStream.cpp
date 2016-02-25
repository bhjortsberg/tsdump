//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <fstream>
#include "TransportStream.h"


TransportStream::TransportStream(const std::string &fileName)
{
    std::ifstream file(fileName);
    char buffer[TSPacket::TS_PACKET_SIZE];
    if (file.is_open())
    {

        while (file.read(buffer, sizeof(buffer)))
        {
            if (buffer[0] == TSPacket::SYNC_BYTE)
            {
                std::vector<unsigned char> target(TSPacket::TS_PACKET_SIZE);
                std::copy(std::begin(buffer), std::end(buffer), std::begin(target));
                packets.push_back(TSPacket(target));
            }
            else
            {
                // TODO: Out of sync
                throw std::runtime_error("Error in sync byte");
            }
        }

    }
}

std::vector<TSPacket> TransportStream::getPackets()
{
    return packets;
}