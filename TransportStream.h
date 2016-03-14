//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TRANSPORTSTREAM_H
#define TSPARSER_TRANSPORTSTREAM_H


#include <iosfwd>
#include <vector>
#include <map>
#include "TSPacket.h"

class TransportStream
{
public:
    TransportStream(const std::string & fileName);
    std::vector<TSPacketPtr> getPackets();
private:
    std::vector<TSPacketPtr> packets;
    std::map<int, TSPacketPtr> m_latest_packets;

    void add_packet(std::vector< unsigned char > vector, int cnt);
};


#endif //TSPARSER_TRANSPORTSTREAM_H
