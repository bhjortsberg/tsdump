//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_IPACKET_H
#define TSPARSE_IPACKET_H

#include <memory>

typedef std::vector<unsigned char> Chunk;

class IPacket
{
public:
    virtual Chunk::const_iterator payload() const = 0;
};

typedef std::shared_ptr<IPacket> IPacketPtr;

#endif //TSPARSE_IPACKET_H
