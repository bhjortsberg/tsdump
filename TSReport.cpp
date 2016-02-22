//
// Created by Björn Hjortsberg on 22/02/16.
//

#include <iostream>
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
        std::string out = m_filter->filter(packet);
        if (out != "")
        {
            std::cout << packet_cnt << " " << out << std::endl;
        }
        packet_cnt++;
    }

    std::cout << m_filter->statistics();

}
