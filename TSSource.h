//
// Created by Björn Hjortsberg on 28/06/16.
//

#ifndef TSPARSE_TSSOURCE_H
#define TSPARSE_TSSOURCE_H

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
};


typedef std::shared_ptr<TSSource> TSSourcePtr;

#endif //TSPARSE_TSSOURCE_H
