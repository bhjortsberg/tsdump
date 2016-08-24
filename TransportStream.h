//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TRANSPORTSTREAM_H
#define TSPARSER_TRANSPORTSTREAM_H


#include <condition_variable>
#include <future>
#include <iosfwd>
#include <vector>
#include <map>
#include <mutex>
#include "TSPacket.h"
#include "TSSource.h"
#include "PMTPacket.h"

class TransportStream
{
public:
    TransportStream(TSSourcePtr sourcePtr,
                    std::condition_variable & cond,
                    std::mutex & mutex);

    std::vector<TSPacketPtr> getPackets();
    std::vector<TSPacketPtr>::iterator find_pat();
    std::vector<TSPacketPtr>::iterator find_pat(const std::vector<TSPacketPtr>::iterator & it );
    std::vector<int> find_pmt_pids(const TSPacketPtr & pat) const;
    std::vector<int> find_pids();
    TSPacketPtr find_pmt(int pid);
    std::vector<PMTPacket> get_pmts();
    bool isDone() const { return m_done; }

private:
    std::vector<TSPacketPtr> m_packets;
    std::vector<TSPacketPtr>::iterator m_currentPacket;
    std::future<std::vector<TSPacketPtr>> m_future;
    TSSourcePtr m_sourcePtr;
    std::condition_variable & m_cond;
    std::mutex & m_mutex;
    bool m_done;
};


#endif //TSPARSER_TRANSPORTSTREAM_H
