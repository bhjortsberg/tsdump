//
// Created by Björn Hjortsberg on 28/06/16.
//

#ifndef TSPARSE_TSSOURCE_H
#define TSPARSE_TSSOURCE_H

#include <string>
#include <vector>

class TSSource
{
public:
    virtual  ~TSSource() {}
    virtual std::vector<TSPacketPtr> operator()() = 0;
    virtual std::vector<TSPacketPtr> getPackets() const = 0;
};


#endif //TSPARSE_TSSOURCE_H
