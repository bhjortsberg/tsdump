//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_REPORTFILTER_H
#define TSPARSE_REPORTFILTER_H


#include <vector>
#include "IFilter.h"

class PacketFilter : public IFilter
{
public:
    void pids(std::vector<int> pids);
    void pts();
    void ebp();
    void rai();
//    virtual void filter(const IPacket & packet);
    virtual std::string filter(const TSPacket & packet) const;
    virtual std::string statistics() const;

private:
    bool m_pts;
    bool m_ebp;
    bool m_rai;
    std::vector<int> m_pids;
};

typedef std::shared_ptr< PacketFilter > PacketFilterPtr;

#endif //TSPARSE_REPORTFILTER_H
