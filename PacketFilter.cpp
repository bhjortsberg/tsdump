//
// Created by Björn Hjortsberg on 2016-02-22.
//

#include <algorithm>
#include "PacketFilter.h"

PacketFilter::PacketFilter():
m_pts(false),
m_ebp(false),
m_rai(false),
m_payloadStart(false)
{

}

bool PacketFilter::show(const TSPacketPtr &packet) const
{

    bool show_packet = filter_pid(packet->pid());

    if (!filter_packet(packet->num()))
    {
        return false;
    }

    if (m_pts && !packet->has_pes_header())
    {
        return false;
    }

    if (m_ebp && !packet->has_ebp())
    {
        return false;
    }

    if (m_rai && !packet->has_random_access_indicator())
    {
        return false;
    }

    if (m_payloadStart && !packet->is_payload_start())
    {
        return false;
    }

    if (!m_rai && !m_ebp &&
            !m_pts && !m_payloadStart && m_pids.empty())
    {
        show_packet = true;
    }

    return show_packet;
}


void PacketFilter::pts()
{
    m_pts = true;
}

void PacketFilter::pids(std::vector< int > pids)
{
    if (pids.empty())
    {
        throw FilterError("Wrong number of arguments for pid listing");
    }
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

bool PacketFilter::filter_pid(int in_pid) const
{
    return filter(in_pid, m_pids);
}

bool PacketFilter::filter_packet(int packet) const
{
    return filter(packet, m_pkts);
}

bool PacketFilter::filter(int f, const std::vector< int > &data) const
{
    if (data.empty()) {
        return true;
    }

    return std::find(std::begin(data), std::end(data), f) != std::end(data);
}

void PacketFilter::payloadStart()
{
    m_payloadStart = true;
}

void PacketFilter::packets(std::vector<int> pkts)
{
    if (pkts.empty())
    {
        throw FilterError("Wrong number of arguments for packet listing");
    }
    m_pkts = pkts;
}

