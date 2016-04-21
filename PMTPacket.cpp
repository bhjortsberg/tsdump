//
// Created by Björn Hjortsberg on 20/04/16.
//

#include <iostream>
#include <iomanip>
#include "PMTPacket.h"
#include "TSPacket.h"


const int AUDIO = 0;
const int VIDEO = 1;

PMTPacket::PMTPacket(const TSPacketPtr &packet):
m_this(packet)
{

}

std::vector< int > PMTPacket::get_elementary_pids()
{
    std::vector<int> pids;

    std::for_each(std::begin(m_elementary_streams), std::end(m_elementary_streams),
                  [&pids](std::pair< unsigned int, unsigned char> thing){
                      pids.emplace_back(thing.first);
                  });
    return pids;
}

bool PMTPacket::is_pmt() const
{
    if (*(m_this->payload() + 7) != 0 ||  *(m_this->payload() + 8) != 0)
    {
        return false;
    }
    return true;
}

unsigned int PMTPacket::program_info_len() const
{
    unsigned short program_info_length = 0;

    program_info_length |= (*(m_this->payload() + 11) & 0x0F) << 8;
    program_info_length |= *(m_this->payload() + 12);

    return program_info_length;
}

std::string PMTPacket::stream_type_string(unsigned int pid) const
{
    std::string type_string;
    auto type = stream_type(pid);
    switch (type)
    {
        case VIDEO:
            type_string = "VIDEO";
            break;
        case AUDIO:
            type_string = "AUDIO";
            break;
        default:
            type_string = "UNKNOWN";

    }

    return type_string;
}

std::vector< int > PMTPacket::elementary_pids()
{
    return std::vector< int >();
}

unsigned char PMTPacket::stream_type(unsigned int pid) const
{
    auto it = m_elementary_streams.find(pid);
    if (it != std::end(m_elementary_streams))
    {
        return it->second;
    }
    return 0;
}

void PMTPacket::parse()
{

    unsigned short section_length;
    section_length = (*(m_this->payload() + 2) & 0x0f) << 8;
    section_length |= *(m_this->payload() + 3);

    auto pit = m_this->payload() + 13 + m_program_info_length;

    unsigned short elemntary_stream_info_length = 0;

    for (int bytes_left = section_length - 13 - m_program_info_length; bytes_left > 0; bytes_left -= 5 + elemntary_stream_info_length)
    {
        unsigned short elementary_pid = 0;
        unsigned char stream_type = *(pit++);

        elementary_pid |= (*(pit++) & 0x1F) << 8;
        elementary_pid |= *(pit++);

        elemntary_stream_info_length = (*(pit++) & 0x0F) << 8;
        elemntary_stream_info_length |= *(pit++);

        pit += elemntary_stream_info_length;
        m_elementary_streams.insert( {elementary_pid, stream_type});
    }

}

Chunk::const_iterator PMTPacket::payload() const
{
    return m_this->payload();
}


PMTPacket parse_pmt(const TSPacketPtr & packet)
{
    PMTPacket pmt(packet);

    if (!pmt.is_pmt()) {
        throw std::runtime_error("Packet not a PMT");
    }

    if (pmt.program_info_len()!= 0) {
        throw std::runtime_error("Program info length not 0");
    }

    pmt.parse();

    return pmt;
}


