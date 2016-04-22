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
    std::string get_packet_string(const TSPacketPtr & packet);
    std::string get_packet_payload_string(const TSPacketPtr & packet);
    std::string get_packet_extra_info_string(const TSPacketPtr & packet);
    std::string get_es_string(const PMTPacket & pmtPacket);
    std::string get_pmt_string(unsigned int pid);
    void print_header();
    void print_summary();
    TSPacketPtr find_prev_pes_packet(const TSPacketPtr & packet);
    IFilterPtr m_filter;
    OutputOptionsPtr m_option;
    TransportStream m_ts;
    std::map<unsigned int, std::vector<int>> m_continuity_error;



};


#endif //TSPARSE_TSREPORT_H
