//
// Created by Björn Hjortsberg on 20/04/16.
//

#include <iostream>
#include <iomanip>
#include "PMTPacket.h"
#include "TSPacket.h"


const std::string stream_type_arr[] = {
        "ITU-T | ISO/IEC Reserved",
        "ISO/IEC 11172 Video",
        "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream",
        "ISO/IEC 11172 Audio",
        "ISO/IEC 13818-3 Audio",
        "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections",
        "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data",
        "ISO/IEC 13522 MHEG",
        "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC",
        "ITU-T Rec. H.222.1",
        "ISO/IEC 13818-6 type A",
        "ISO/IEC 13818-6 type B",
        "ISO/IEC 13818-6 type C",
        "ISO/IEC 13818-6 type D",
        "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary",
        "ISO/IEC 13818-7 Audio with ADTS transport syntax",
        "ISO/IEC 14496-2 Visual",
        "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1",
        "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets",
        "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.",
        "ISO/IEC 13818-6 Synchronized Download Protocol"
};

const std::string stream_type_reserved = "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved";
const std::string stream_type_user_private = "User Private";

PMTPacket::PMTPacket(const TSPacketPtr &packet):
m_this(packet)
{

}

std::vector< int > PMTPacket::get_elementary_pids() const
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

    if (type < 0x15) {
        type_string = stream_type_arr[type];
    } else if (type < 0x80) {
        type_string = stream_type_reserved;
    } else {
        type_string = stream_type_user_private;
    }

    return type_string;
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

    m_program_info_length = program_info_len();
    auto pit = m_this->payload() + 13 + m_program_info_length;

    unsigned short elemntary_stream_info_length = 0;

    int bytes_left2 = section_length - 13 - m_program_info_length;
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


