//
// Created by bjorn on 2017-11-21.
//

#pragma once

#include <mutex>
#include <condition_variable>
#include "TSSource.h"

class MulticastSource : public TSSource
{
public:
    MulticastSource(const std::string& source, std::condition_variable& cond, std::mutex& mutex);
    virtual ~MulticastSource();
    virtual std::vector<TSPacketPtr> getPackets() override;
    virtual std::vector<TSPacketPtr> doRead() override;
    virtual bool isDone() override ;

private:
    void join(const std::string & addr);
    void add_packet(std::vector< unsigned char > & raw_packet, int cnt);

    std::vector<TSPacketPtr> m_packets;
    std::map<int, TSPacketPtr> m_latest_packets;
    std::condition_variable & m_partially_read;
    std::mutex & m_mutex;
    std::string m_addr;
    uint16_t m_port;
    int m_sock;
    bool m_joined;
    bool m_done;
};


