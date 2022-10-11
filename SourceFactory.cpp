//
// Created by Björn Hjortsberg on 2017-12-20.
//

#include <netdb.h>
#include <memory>
#include "SourceFactory.h"
#include "FileSource.h"
#include "MulticastSource.h"

TSSourcePtr SourceFactory::create(const std::string& source,
                                  std::condition_variable& cond,
                                  std::mutex& mutex)
{
    struct addrinfo *addrInfo;
    auto ipEndPos = source.find(":");
    std::string ipStr;
    if (ipEndPos != std::string::npos)
    {
        ipStr = source.substr(0, ipEndPos);
    }
    if (getaddrinfo(ipStr.c_str(), nullptr, nullptr, &addrInfo) == 0)
    {
        // Servername resolved asume multicast source
        return std::make_shared<MulticastSource>(source, cond, mutex);
    }
    else
    {
        // Failed to resolv host - asume file
        return std::make_shared<FileSource>(source, cond, mutex);
    }
}