//
// Created by Björn Hjortsberg on 20/04/16.
//

#pragma once

#include <map>
#include <vector>
#include "IPacket.h"
#include "TSPacket.h"

class PMTPacket;
PMTPacket parse_pmt(const TSPacketPtr & packet);

class ElementaryStream
{
    unsigned int pid;
    unsigned char type;
};

class PMTPacket : public IPacket
{
public:
    PMTPacket(const TSPacketPtr & packet);
    std::vector<int> get_elementary_pids() const;

    bool is_pmt() const;
    unsigned int program_info_len() const;
    unsigned char stream_type(unsigned int pid) const;
    std::string stream_type_string(unsigned int pid) const;
    void parse();
    virtual Chunk::const_iterator payload() const;

private:
    IPacketPtr m_this;
    std::map<unsigned int, unsigned char> m_elementary_streams;
    unsigned short m_program_info_length;

};

