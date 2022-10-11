//
// Created by bjorn on 2017-11-21.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include "MulticastSource.h"
#include "TSPacket.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>


namespace {
uint32_t readNetworkPackets(
        int sock,
        unsigned char *destinationBuffer,
        size_t bufSize,
        fd_set readSet,
        struct sockaddr *address,
        sigset_t* mask);
}

MulticastSource::MulticastSource(const std::string &source, std::condition_variable &cond, std::mutex &mutex)
:TSSourceAbstract(mutex),
 mPartiallyRead(cond),
 mJoined(false)
{
    size_t portDelimiter = source.find(':');
    if (portDelimiter != std::string::npos)
    {
        mAddr = source.substr(0, portDelimiter);
        auto portStr = source.substr(portDelimiter + 1, source.size() - portDelimiter - 1);
        uint16_t port = atoi(portStr.c_str());
        mPort = htons(port);
        mSock = socket(AF_INET, SOCK_DGRAM, 0);
        join(mAddr);
    } else {
        throw std::runtime_error("ERROR: Invalid address format should be <ip>:<port>");
    }
}

void MulticastSource::join(const std::string & addr)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(addr.c_str());
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (!setsockopt(mSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        mJoined = true;
    }

    // Set non blocking socket
    uint32_t nonblock = 1;
    ioctl(mSock, FIONBIO, &nonblock);
}

// Implement according to code example in:
// https://stackoverflow.com/questions/6962150/catching-signals-while-reading-from-pipe-with-select

std::vector<TSPacketPtr> MulticastSource::read() {
    fd_set readSet;
    struct sockaddr_in address;
    address.sin_port = mPort;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    FD_ZERO(&readSet);
    if (bind(mSock, (struct sockaddr*)&address, sizeof(address)))
    {
        perror("bind error");
        throw std::runtime_error("Bind error: " + std::string(strerror(errno)));
    }

    FD_SET(mSock, &readSet);

    std::vector<unsigned char> rawPacket(TSPacket::TS_PACKET_SIZE * 10);
    sigset_t mask;
    sigset_t origMask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, &origMask) < 0) {
        perror ("sigprocmask");
    }

    uint32_t aPos = 0;
    while (not mStop && aPos == 0)
    {
        aPos = readNetworkPackets(
                mSock,
                rawPacket.data(),
                rawPacket.capacity(),
                readSet,
                (struct sockaddr *) &address, &origMask);
    }
    auto[bytesLeft, multiPackets] = findSynchAndAddPackets(aPos, rawPacket);

    while (not mStop)
    {
        uint32_t pos = bytesLeft;
        uint32_t bytes = readNetworkPackets(
                mSock,
                multiPackets.data() + pos,
                multiPackets.capacity() - pos,
                readSet,
                (struct sockaddr *) &address, &origMask);
        if (bytes == 0) {
            // No data
            continue;
        }
        pos += bytes;
        if (pos > multiPackets.size())
        {
            // Destination buffer is full
            perror("recvfrom: Buffer full");
            return mPackets;
        }

        auto numberOfPackets = pos / TSPacket::TS_PACKET_SIZE;
        addAllPacketsAndResync(numberOfPackets, multiPackets);

        bytesLeft = 0;
        // Notify that packets has been read
        mPartiallyRead.notify_one();
    }

    mDone = true;
    // Release the waiting thread
    mPartiallyRead.notify_one();
    std::vector<TSPacketPtr> packets;
    {
        std::lock_guard< std::mutex > lock(mMutex);
        packets = mPackets;
    }
    return packets;
}


namespace {
uint32_t readNetworkPackets(
        int sock,
        unsigned char* destinationBuffer,
        size_t bufSize,
        fd_set readSet,
        struct sockaddr* address,
        sigset_t* mask)
{
    socklen_t len;
    uint32_t numBytes = 0;

    struct timespec timeout;
    int result = 0;
    timeout.tv_sec = 0;
    timeout.tv_nsec = 1000000L; // Poll
    result = pselect(FD_SETSIZE, &readSet, NULL, NULL, &timeout, mask);
    // Timeout continue to poll
    if (result == -1 && errno == EINTR) {
        std::cout << "pselect signal caught\n";
    }

    if (result == 1 && FD_ISSET(sock, &readSet))
    {
        ssize_t bytes;
        while ((bytes = recvfrom(sock,
                                 destinationBuffer + numBytes,
                                 bufSize - numBytes,
                                 0,
                                 address,
                                 &len)) > 0)
        {
            numBytes += bytes;
        }
        if (bytes < 0 && errno != EAGAIN)
        {
            // Failed to read but there are still data
            std::cout << "Fail to read socket\n";
        }
    }
    return numBytes;
}

}