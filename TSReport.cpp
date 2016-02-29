//
// Created by Björn Hjortsberg on 22/02/16.
//

#include <iostream>
#include <sstream>
#include "TSReport.h"

TSReport::TSReport(const TransportStream &tstream, const IFilterPtr & filter):
m_ts(tstream),
m_filter(filter)
{

}

void TSReport::report()
{
    int packet_cnt = 0;
    for (const auto & packet : m_ts.getPackets())
    {
        if (m_filter->show(packet))
        {
            std::cout << packet_cnt << " " << get_packet_string(packet);
        }
        packet_cnt++;
    }

//    std::cout << m_filter->statistics();

}

std::string TSReport::get_packet_string(const TSPacket & packet)
{
    std::stringstream packet_string;

    packet_string << packet.pid() << " ";
    if (packet.has_pes_header())
    {
        packet_string << std::hex << "0x" << packet.pes_header().get_pts() << std::dec << " " << packet.pes_header().get_pts_str() << " ";
    }

    if (packet.has_adaption_field() && packet.has_ebp())
    {
        packet_string << "EBP ";
    }

    if (packet.has_random_access_indicator())
    {
        packet_string << "RAI ";
    }

    packet_string << std::endl;

    return packet_string.str();

}
