//
// Created by Björn Hjortsberg on 22/02/16.
//

#include "PacketFilter.h"


bool PacketFilter::show(const TSPacket &packet) const
{

    bool show_packet = filter_pid(packet.pid());

    if (m_pts && !packet.has_pes_header())
    {
        return false;
    }

    if (m_ebp && !packet.has_ebp())
    {
        return false;
    }

    if (m_rai && !packet.has_random_access_indicator())
    {
        return false;
    }

    if (!m_rai && !m_ebp && !m_pts && m_pids.empty())
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
    bool ret = false;

    if (m_pids.empty()) {
        ret = true;
    }
    for (const auto & pid : m_pids)
    {
        if (pid == in_pid)
        {
            ret = true;
            break;
        }
    }
    return ret;
}
