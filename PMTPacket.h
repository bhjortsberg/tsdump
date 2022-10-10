//
// Created by Björn Hjortsberg on 2016-04-20.
//

#pragma once

#include <map>
#include <vector>
#include "IPacket.h"
#include "TSPacket.h"

class PMTPacket;
PMTPacket parse_pmt(const TSPacketPtr & packet);

// TODO: Use this class for mElementaryStream
class ElementaryStream
{
    unsigned int pid;
    unsigned char type;
};

class PMTPacket : public IPacket
{
public:
    PMTPacket(const TSPacketPtr & packet);
    std::vector<int> getElementaryPids() const;

    bool isPmt() const;
    unsigned int programInfoLength() const;
    unsigned char streamType(unsigned int pid) const;
    std::string streamTypeString(unsigned int pid) const;
    void parse();
    virtual Chunk::const_iterator payload() const;

private:
    IPacketPtr mThis;
    std::map<unsigned int, unsigned char> mElementaryStreams;
    unsigned short mProgramInfoLength;

};

