//
// Created by Björn Hjortsberg on 22/02/16.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include "TSReport.h"

TSReport::TSReport(TransportStream &tstream,
                   const IFilterPtr & filter,
                   const OutputOptionsPtr & option):
m_ts(tstream),
m_filter(filter),
m_option(option)
{

}

void TSReport::report()
{
    if (m_option->listPidsOnly()) {
        print_pid_info();
        return;
    }

    m_packetCount = 0;
    print_header();
    while (true)
    {

        for (const auto& packet : m_ts.getPackets())
        {
            ++m_packetCount;
            if (m_filter->show(packet))
            {
                std::cout << get_packet_string(packet);

                if (m_option->printExtraInfo())
                {
                    std::cout << get_packet_extra_info_string(packet) << "\n";
                }
                if (m_option->printPayload())
                {
                    std::cout << get_packet_payload_string(packet) << "\n";
                }

                if (!packet->continuity())
                {
                    auto it = m_continuity_error.find(packet->pid());
                    if (it != std::end(m_continuity_error))
                    {
                        it->second.push_back(packet->num());
                        m_continuity_error.emplace(packet->pid(), it->second);
                    }
                    else
                    {
                        std::vector<int> v {packet->num()};
                        m_continuity_error.emplace(packet->pid(), v);
                    }
                }
            }
        }

        if (m_ts.isDone())
        {
            break;
        }

    }

    print_summary();

//    std::cout << m_filter->statistics();

}

std::string TSReport::get_packet_string(const TSPacketPtr & packet)
{
    std::stringstream packet_string;

    packet_string << packet->num() << "\t" << packet->pid() << "\t" << packet->continuity_count() << "\t\t";
    if (packet->has_pes_header())
    {
        packet_string << std::hex << "0x"
                    << packet->pes_header().get_pts()
                    << std::dec << "\t"
                    << packet->pes_header().get_pts_str() << "\t"
                    << packet->pes_header().get_length() << "\t";
    }
    else {
        packet_string << "\t\t\t\t";
    }


    if (packet->has_adaptation_field() && packet->has_ebp())
    {
        packet_string << "EBP\t";
    }
    else {
        packet_string << "   \t";
    }

    if (packet->has_random_access_indicator())
    {
        packet_string << "RAI\t";
    }
    else {
        packet_string << "   \t";
    }

    if (packet->is_payload_start())
    {
        packet_string << "Start\t";
    }

    if (packet->has_pes_header())
    {
        TSPacketPtr prev_pes_packet = find_prev_pes_packet(packet);
        if (prev_pes_packet)
        {
            int pts_diff = packet->pes_header().get_pts() - prev_pes_packet->pes_header().get_pts();
            if (pts_diff > 20000)
            {
                packet_string << "PTS diff: " << pts_diff;
            }
        }
    }

    packet_string << "\n";

    return packet_string.str();

}

void TSReport::print_header()
{
    std::cout << "Packet\tPID\tContinuity\tpts hex\t\tpts wall\tSize\tEBP\tRAI\tPayload\n";
    std::cout << "-----------------------------------------------------------------------------------------------\n";
}

std::string TSReport::get_packet_payload_string(const TSPacketPtr &packet)
{
    std::stringstream payload_str;
    payload_str << std::hex << "\n\t\t";

    int byte_cnt = 0;
    std::stringstream line;
    std::string asciiline = "";
    for (auto & p : packet->get_payload())
    {
        line << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(p) << " ";
        asciiline += (((p > 31) && (p < 127)) ? p : '.');

        ++byte_cnt;

        if (byte_cnt % 8 == 0) {
            line << " ";
            asciiline += " ";
        }
        if (byte_cnt % 16 == 0) {
            payload_str << line.str() << " | " << asciiline << "\n\t\t";
            line.str("");
            asciiline = "";
        }
    }
    payload_str << std::left << std::setfill(' ') << std::setw(16*3 + 2) << line.str() << " | " << asciiline << "\n";

    return payload_str.str();
}

std::string TSReport::get_packet_extra_info_string(const TSPacketPtr &packet)
{
    std::stringstream adaptation_str;
    std::stringstream pes_str;
    pes_str << "\n";

    if (packet->has_adaptation_field())
    {
        adaptation_str << "\n" << packet->adaptation_field().print_str();
    }

    if (packet->has_pes_header())
    {
        pes_str << packet->pes_header().print_str();
    }

    return adaptation_str.str() + pes_str.str();

}

void TSReport::print_summary()
{
    std::cout << std::endl << "Summary:" << std::endl;
    std::cout << "\tNumber of packets: " << m_packetCount << std::endl;

    if (!m_continuity_error.empty())
    {
        std::cout << "\tContinutity count error in:" << std::endl;
        for (const auto & a : m_continuity_error)
        {
            bool first = true;
            std::cout << "\t\tpid: " << a.first << " packet numbers: ";
            for (const auto & b : a.second)
            {
                std::cout << (first ? std::to_string(b) : ", " + std::to_string(b));
                first = false;
            }
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
}

TSPacketPtr TSReport::find_prev_pes_packet(const TSPacketPtr & packet)
{
    TSPacketPtr curr_packet = packet;

    while ((curr_packet = curr_packet->get_prev()) != nullptr)
    {
        if (curr_packet->has_pes_header())
        {
            break;
        }
    }

    return curr_packet;
}

void TSReport::print_pid_info()
{
    std::cout << std::setw(6) << "Pid\tType" << std::endl;
    std::cout << "=====================" << std::endl;
    auto pat = m_ts.find_pat();
    auto pmt_pids = m_ts.find_pmt_pids(*pat);
    for (auto p : pmt_pids) {
        std::cout << get_pmt_string(p);
    }

    auto pmts = m_ts.get_pmts();
    for (auto p : pmts) {
        std::cout << get_es_string(p) << std::endl;
    }
}

std::string TSReport::get_pmt_string(unsigned int pid)
{
    std::stringstream ss;
    ss << std::setw(4) << std::right << std::dec << std::setfill(' ') << pid << "\t";
    ss << std::left << "PMT " << std::endl;

    return ss.str();
}

std::string TSReport::get_es_string(const PMTPacket &pmtPacket)
{
    std::stringstream ss;
    for (auto p : pmtPacket.get_elementary_pids())
    {
        ss << std::setw(4) << std::right << std::dec << std::setfill(' ') << p << "\t";
        ss << "0x" << std::setw(2) << std::hex << std::setfill('0') << static_cast<int>(pmtPacket.stream_type(p));
        ss << std::left << " " << pmtPacket.stream_type_string(p) << std::endl;
    }

    return ss.str();
}
