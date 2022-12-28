//
// Created by Björn Hjortsberg on 2016-04-20.
//

#pragma once

#include <map>
#include <vector>
#include "IPacket.h"
#include "TSPacket.h"

class PMTPacket;
PMTPacket parsePmt(const TSPacketPtr & packet);

class ElementaryStream
{
public:
    ElementaryStream(uint32_t pid, uint32_t type): pid(pid), type(type)
    {}
    unsigned int pid;
    unsigned char type;
};

class PMTPacket : public IPacket
{
public:
    explicit PMTPacket(const TSPacketPtr & packet);
    std::vector<int> getElementaryPids() const;

    bool isPmt() const;
    unsigned int programInfoLength() const;
    unsigned char streamType(unsigned int pid) const;
    std::string streamTypeString(unsigned int pid) const;
    void parse();
    virtual const Chunk::const_iterator& payload() const override;

private:
    IPacketPtr mThis;
    std::vector<ElementaryStream> mElementaryStreams;
    unsigned short mProgramInfoLength;

};