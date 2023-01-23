//
// Created by Björn Hjortsberg on 28/06/16.
//
#ifndef TSPARSE_FILESOURCE_H
#define TSPARSE_FILESOURCE_H

#include <condition_variable>
#include <vector>

#include "TSPacket.h"
#include "TSSource.h"


class FileSource : public TSSource
{
public:
    FileSource(const std::string & source,
               std::condition_variable & cond,
               std::mutex & mutex);
    virtual ~FileSource() {}

    virtual std::vector<TSPacketPtr> getPackets() override;
    virtual std::vector<TSPacketPtr> doRead() override;
    virtual bool isDone() override;
    virtual void stop() override;

protected:
//    void add_packet(std::vector<unsigned char > & raw_packet, int cnt);
    std::string m_filename;
    std::vector<TSPacketPtr> m_packets;
    std::map<int, TSPacketPtr> m_latest_packets;
    std::condition_variable & m_partially_read;
    std::mutex & m_mutex;
    std::vector<TSPacketPtr>::iterator m_lastRetreivedPacket;
    std::vector<TSPacketPtr>::iterator m_nullIterator;

    std::vector<TSPacketPtr> read();
    void add_packet(std::vector< unsigned char > & raw_packet);
    void add_packet(const std::vector< unsigned char >::const_iterator& packet_start);
    std::tuple<uint32_t, std::vector<uint8_t>> findSynchAndAddPackets(uint32_t initialPos, const std::vector<unsigned char>& raw_packet);
    void addAllPacketsAndResync(uint32_t numberOfPackets, const std::vector<uint8_t>& multi_packets);
    bool m_done = false;
    bool m_stop = false;
    uint32_t mPacketCount = 0;
};

typedef std::shared_ptr<FileSource> FileSourcePtr;

#endif //TSPARSE_FILESOURCE_H
