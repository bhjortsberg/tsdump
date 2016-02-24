//
// Created by Björn Hjortsberg on 22/02/16.
//

#include <sstream>
#include "PacketFilter.h"


// TODO: This is really ugly and unmantainable
// Algorithm:
// Should packet be printed or not?
// If no filter is set. Whole Packet should be printed.
// if rai filter and rai - print pid pts and rai
// if ebp filter and ebp - print pid pts and ebp
// if pts filter and pts - print pts
//
std::string PacketFilter::filter(const TSPacket &packet) const
{
    std::stringstream pts_string;
    std::stringstream output;
    std::stringstream pid_str;
    bool pts_reported = false;

    pid_str << packet.pid() << " ";
    if (packet.has_pes_header())
    {
        pts_string << std::hex << "0x" << packet.pes_header().get_pts() << std::dec << " " << packet.pes_header().get_pts_str() << " ";
    }

    if (!m_pids.empty())
    {
        if (static_cast<int>(m_pids[0]) != packet.pid()){
            return output.str();
        }

    }

    if (m_pts)
    {
        if (packet.has_pes_header())
        {
            output << pid_str.str() << pts_string.str();
            pts_reported = true;
        }
    }

    if (m_ebp)
    {
        if (packet.has_adaption_field() && packet.has_ebp())
        {
            if (!pts_reported && packet.has_pes_header()) {
                output << pid_str.str() <<  pts_string.str();
                pts_reported = true;
            }
            output <<  "EBP ";
        }
    }

    if (m_rai)
    {
        if (packet.has_random_access_indicator())
        {
            if (!pts_reported && packet.has_pes_header()) {
                output << pid_str.str() << pts_string.str();
            }
            output <<  "RAI ";
        }
    }

    if (!m_rai && !m_ebp && !m_pts)
    {
        output << pid_str.str();
        if (packet.has_pes_header())
        {
            output << pts_string.str();
        }

        if (packet.has_adaption_field())
        {
            if (packet.has_ebp())
            {
                output << "EBP ";
            }

            if (packet.has_random_access_indicator())
            {
                output << "RAI ";
            }
        }

        if (output.str() == "")
        {
            output  << " ";
        }
    }

    return output.str();
}

void PacketFilter::pts()
{
    m_pts = true;
}

void PacketFilter::pids(std::vector< int > pids)
{
    m_pids = pids;
}

void PacketFilter::ebp()
{
    m_ebp = true;
}

void PacketFilter::rai()
{
    m_rai = true;
}

std::string PacketFilter::statistics() const
{
    std::stringstream statistics;
    statistics << "===============================================" << std::endl;

    return statistics.str();
}
