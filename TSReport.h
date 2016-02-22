//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_TSREPORT_H
#define TSPARSE_TSREPORT_H

#include "TransportStream.h"
#include "IFilter.h"

class TSReport
{
public:
    TSReport(const TransportStream & tstream, const IFilterPtr & filter);

    void report();
private:
    IFilterPtr m_filter;
    TransportStream m_ts;


};


#endif //TSPARSE_TSREPORT_H
