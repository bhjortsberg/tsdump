//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TRANSPORTSTREAM_H
#define TSPARSER_TRANSPORTSTREAM_H


#include <iosfwd>
#include <vector>
#include <map>
#include "TSPacket.h"
#include "PMTPacket.h"

class TransportStream
{
public:
    TransportStream(const std::string & fileName);
    std::vector<TSPacketPtr> getPackets();
    std::vector<TSPacketPtr>::iterator find_pat();
    std::vector<TSPacketPtr>::iterator find_pat(const std::vector<TSPacketPtr>::iterator & it );
    std::vector<int> find_pmt_pids(const TSPacketPtr & pat) const;
    std::vector<int> find_pids();
    TSPacketPtr find_pmt(int pid);
    std::vector<PMTPacket> get_pmts();

private:
    std::vector<TSPacketPtr> packets;
    std::map<int, TSPacketPtr> m_latest_packets;

    void add_packet(std::vector< unsigned char > vector, int cnt);
};


#endif //TSPARSER_TRANSPORTSTREAM_H
