//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_REPORTFILTER_H
#define TSPARSE_REPORTFILTER_H


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
    bool filter_pid(int pid) const;
    bool filter_packet(int packet) const;
    bool m_pts;
    bool m_ebp;
    bool m_rai;
    bool m_payloadStart;
    std::vector<int> m_pids;
    std::vector<int> m_pkts;
};

typedef std::shared_ptr< PacketFilter > PacketFilterPtr;

#endif //TSPARSE_REPORTFILTER_H
