//
// Created by Björn Hjortsberg on 2016-02-22.
//

#pragma once

#include <string>
#include <vector>
#include "IFilter.h"

class PacketFilter : public IFilter
{
public:
    PacketFilter();
    void pids(std::vector<int> pids);
    void pts();
    void ebp();
    void rai();
    void payloadStart();
    void packets(std::vector<int> pkts);
    virtual bool show(const TSPacketPtr &packet) const;

private:
    bool filter(int f, const std::vector<int> & data) const;
    bool filterPid(int pid) const;
    bool filterPacket(int packet) const;
    bool mPts;
    bool mEbp;
    bool mRai;
    bool mPayloadStart;
    std::vector<int> mPids;
    std::vector<int> mPackets;
};

using PacketFilterPtr = std::shared_ptr< PacketFilter >;

