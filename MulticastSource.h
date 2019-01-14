//
// Created by bjorn on 2017-11-21.
//

#pragma once

#include <mutex>
#include <condition_variable>
#include "TSSourceAbstract.h"

class MulticastSource : public TSSourceAbstract
{
public:
    MulticastSource(const std::string& source, std::condition_variable& cond, std::mutex& mutex);
    virtual ~MulticastSource() = default;

protected:
    std::vector<TSPacketPtr> read() override;
    void join(const std::string & addr);

    std::condition_variable& mPartiallyRead;
    std::string mAddr;
    uint16_t mPort;
    int mSock;
    bool mJoined;
};


