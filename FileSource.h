//
// Created by Björn Hjortsberg on 28/06/16.
//
#pragma once

#include <condition_variable>
#include <vector>

#include "TSPacket.h"
#include "TSSourceAbstract.h"


class FileSource : public TSSourceAbstract
{
public:
    FileSource(const std::string & source,
               std::condition_variable & cond,
               std::mutex & mutex);
    virtual ~FileSource() {}

protected:
//    void add_packet(std::vector<unsigned char > & raw_packet, int cnt);
    std::vector<TSPacketPtr> read() override;

    std::string mFilename;
    std::condition_variable & mPartiallyRead;

};

using FileSourcePtr = std::shared_ptr<FileSource>;

