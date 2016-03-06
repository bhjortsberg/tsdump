//
// Created by Björn Hjortsberg on 22/02/16.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include "TSReport.h"

TSReport::TSReport(const TransportStream &tstream, const IFilterPtr & filter, const OutputOptionsPtr & option):
m_ts(tstream),
m_filter(filter),
m_option(option)
{

}

void TSReport::report()
{
    print_header();
    for (const auto & packet : m_ts.getPackets())
    {
        if (m_filter->show(packet))
        {
            std::cout << get_packet_string(packet);
            if (m_option->printPayload())
            {
                std::cout << get_packet_payload_string(packet);
            }
        }
    }

//    std::cout << m_filter->statistics();

}

std::string TSReport::get_packet_string(const TSPacket & packet)
{
    std::stringstream packet_string;

    packet_string << packet.num() << "\t\t" << packet.pid() << "\t";
    if (packet.has_pes_header())
    {
        packet_string << std::hex << "0x" << packet.pes_header().get_pts() << std::dec << "\t" << packet.pes_header().get_pts_str() << "\t";
    }
    else {
        packet_string << "\t\t\t\t";
    }


    if (packet.has_adaption_field() && packet.has_ebp())
    {
        packet_string << "EBP\t";
    }
    else {
        packet_string << "   \t";
    }

    if (packet.has_random_access_indicator())
    {
        packet_string << "RAI\t";
    }
    else {
        packet_string << "   \t";
    }

    if (packet.is_payload_start())
    {
        packet_string << "Start\t";
    }

    packet_string << std::endl;

    return packet_string.str();

}

void TSReport::print_header()
{
    std::cout << "Packet No.\tPID\tpts hex\t\tpts wall\tEBP\tRAI\tPayload" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
}

std::string TSReport::get_packet_payload_string(const TSPacket &packet)
{
    std::stringstream payload_str;
    payload_str << std::hex << "\t\t";

    int byte_cnt = 0;
    std::stringstream line, asciiline;
    for (auto & p : packet.get_payload())
    {
        payload_str << std::setfill('0') << std::setw(2)  << static_cast<int>(p) << " ";
        asciiline << p;
        if (++byte_cnt % 16 == 0) {
            payload_str << std::endl << "\t\t";
        }
    }
    payload_str << std::endl;
    return payload_str.str();
}
