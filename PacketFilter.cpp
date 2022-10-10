//
// Created by Björn Hjortsberg on 2016-02-22.
//

#include <algorithm>
#include "PacketFilter.h"

PacketFilter::PacketFilter():
        mPts(false),
        mEbp(false),
        mRai(false),
        mPayloadStart(false)
{

}

bool PacketFilter::show(const TSPacketPtr &packet) const
{

    bool show_packet = filterPid(packet->pid());

    if (!filterPacket(packet->number()))
    {
        return false;
    }

    if (mPts && !packet->hasPesHeader())
    {
        return false;
    }

    if (mEbp && !packet->hasEbp())
    {
        return false;
    }

    if (mRai && !packet->hasRandomAccessIndicator())
    {
        return false;
    }

    if (mPayloadStart && !packet->isPayloadStart())
    {
        return false;
    }

    if (!mRai && !mEbp &&
        !mPts && !mPayloadStart && mPids.empty())
    {
        show_packet = true;
    }

    return show_packet;
}


void PacketFilter::pts()
{
    mPts = true;
}

void PacketFilter::pids(std::vector< int > pids)
{
    if (pids.empty())
    {
        throw FilterError("Wrong number of arguments for pid listing");
    }
    mPids = pids;
}

void PacketFilter::ebp()
{
    mEbp = true;
}

void PacketFilter::rai()
{
    mRai = true;
}

bool PacketFilter::filterPid(int pid) const
{
    return filter(pid, mPids);
}

bool PacketFilter::filterPacket(int packet) const
{
    return filter(packet, mPackets);
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
    mPayloadStart = true;
}

void PacketFilter::packets(std::vector<int> pkts)
{
    if (pkts.empty())
    {
        throw FilterError("Wrong number of arguments for packet listing");
    }
    mPackets = pkts;
}

