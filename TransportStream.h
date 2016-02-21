//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TRANSPORTSTREAM_H
#define TSPARSER_TRANSPORTSTREAM_H


#include <iosfwd>
#include <vector>
#include "TSPacket.h"

class TransportStream
{
public:
    TransportStream(const std::string & fileName);
    std::vector<TSPacket> getPackets();
private:
    std::vector<TSPacket> packets;
};


#endif //TSPARSER_TRANSPORTSTREAM_H
