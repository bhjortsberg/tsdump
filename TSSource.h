//
// Created by Björn Hjortsberg on 2016-06-28.
//

#pragma once

#include <string>
#include <vector>
#include "TSPacket.h"

class TSSource
{
public:
    virtual  ~TSSource() {}
    virtual std::vector<TSPacketPtr> getPackets() = 0;
    virtual std::vector<TSPacketPtr> doRead() = 0;
    virtual bool isDone() = 0;
    virtual void stop() = 0;
    virtual bool isStopped() = 0;
};

using TSSourcePtr = std::shared_ptr<TSSource>;