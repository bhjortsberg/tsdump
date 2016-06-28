//
// Created by Björn Hjortsberg on 28/06/16.
//
#ifndef TSPARSE_FILESOURCE_H
#define TSPARSE_FILESOURCE_H

#include <vector>

#include "TSPacket.h"
#include "TSSource.h"


class FileSource : public TSSource
{
public:
    FileSource(const std::string & source);
    virtual ~FileSource() {}

    virtual std::vector<TSPacketPtr> operator()();
    virtual std::vector<TSPacketPtr> getPackets() const;

protected:
    void add_packet(std::vector<unsigned char > & raw_packet, int cnt);
    std::string mFilename;
    std::vector<TSPacketPtr> packets;
    std::map<int, TSPacketPtr> m_latest_packets;

};


#endif //TSPARSE_FILESOURCE_H
