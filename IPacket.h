//
// Created by Björn Hjortsberg on 22/02/16.
//

#pragma once

#include <memory>

using Chunk = std::vector<unsigned char>;

class IPacket
{
public:
    virtual Chunk::const_iterator payload() const = 0;
};

using IPacketPtr = std::shared_ptr<IPacket>;

