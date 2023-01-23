//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_IPACKET_H
#define TSPARSE_IPACKET_H

#include <memory>

using Chunk = std::vector<unsigned char>;

class IPacket
{
public:
    virtual Chunk::const_iterator payload() const = 0;
};

using IPacketPtr = std::shared_ptr<IPacket>;

#endif //TSPARSE_IPACKET_H
