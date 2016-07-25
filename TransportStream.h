//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TRANSPORTSTREAM_H
#define TSPARSER_TRANSPORTSTREAM_H


#include <condition_variable>
//#include <future>
#include <iosfwd>
#include <vector>
#include <map>
#include <mutex>
#include "TSPacket.h"
#include "PMTPacket.h"

class TransportStream
{
public:
    TransportStream(const std::string & fileName,
                    std::condition_variable & cond,
                    std::mutex & mutex);

    std::vector<TSPacketPtr> getPackets();
    std::vector<TSPacketPtr>::iterator find_pat();
    std::vector<TSPacketPtr>::iterator find_pat(const std::vector<TSPacketPtr>::iterator & it );
    std::vector<int> find_pmt_pids(const TSPacketPtr & pat) const;
    std::vector<int> find_pids();
    TSPacketPtr find_pmt(int pid);
    std::vector<PMTPacket> get_pmts();

private:
    std::vector<TSPacketPtr> packets;

//    std::future<std::vector<TSPacketPtr>> m_future;
};


#endif //TSPARSER_TRANSPORTSTREAM_H
