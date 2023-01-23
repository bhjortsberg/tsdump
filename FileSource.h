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
               std::mutex & mutex,
               std::vector<TSPacketPtr> & packets);
    virtual ~FileSource() {}

    virtual std::vector<TSPacketPtr> operator()();
    virtual std::vector<TSPacketPtr> getPackets() const;

protected:
    void add_packet(std::vector<unsigned char > & raw_packet, int cnt);
    std::string m_filename;
    std::vector<TSPacketPtr> & m_packets;
    std::map<int, TSPacketPtr> m_latest_packets;
    std::condition_variable & m_partially_read;
    std::mutex & m_mutex;

};


#endif //TSPARSE_FILESOURCE_H
