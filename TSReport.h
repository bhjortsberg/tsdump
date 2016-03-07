//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_TSREPORT_H
#define TSPARSE_TSREPORT_H

#include "TransportStream.h"
#include "IFilter.h"
#include "OutputOptions.h"

class TSReport
{
public:
    TSReport(const TransportStream & tstream, const IFilterPtr & filter, const OutputOptionsPtr & option);
    void report();

private:
    std::string get_packet_string(const TSPacket & packet);
    std::string get_packet_payload_string(const TSPacket & packet);
    std::string get_packet_extra_info_string(const TSPacket & packet);
    void print_header();
    IFilterPtr m_filter;
    OutputOptionsPtr m_option;
    TransportStream m_ts;


};


#endif //TSPARSE_TSREPORT_H
