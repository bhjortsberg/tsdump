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
uint32_t read_net_packets(
        int sock,
        unsigned char *dest_buf,
        size_t buf_size,
        fd_set read_set,
        struct sockaddr *addr,
        sigset_t* mask);
}

MulticastSource::MulticastSource(const std::string &source, std::condition_variable &cond, std::mutex &mutex)
:TSSourceAbstract(mutex),
 mPartiallyRead(cond),
 mJoined(false)
{
    size_t port_delim = source.find(':');
    if (port_delim != std::string::npos)
    {
        mAddr = source.substr(0, port_delim);
        auto port_str = source.substr(port_delim + 1, source.size() - port_delim -1);
        uint16_t port = atoi(port_str.c_str());
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

std::vector<TSPacketPtr> MulticastSource::read() {
    fd_set readSet;
    struct sockaddr_in addr;
    addr.sin_port = mPort;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    FD_ZERO(&readSet);
    if (bind(mSock, (struct sockaddr*)&addr, sizeof(addr)))
    {
        perror("bind error");
        throw std::runtime_error("Bind error: " + std::string(strerror(errno)));
    }

    FD_SET(mSock, &readSet);

    std::vector<unsigned char> raw_packet(TSPacket::TS_PACKET_SIZE * 10);
    sigset_t mask;
    sigset_t orig_mask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror ("sigprocmask");
    }

    uint32_t aPos = read_net_packets(
            mSock,
            raw_packet.data(),
            raw_packet.capacity(),
            readSet,
            (struct sockaddr *) &addr, &orig_mask);

    auto [bytes_left, multi_packets] = findSynchAndAddPackets(aPos, raw_packet);

    while (not mStop)
    {
        uint32_t pos = bytes_left;
        pos += read_net_packets(
                mSock,
                multi_packets.data() + pos,
                multi_packets.capacity() - pos,
                readSet,
                (struct sockaddr *) &addr, &orig_mask);
        if (pos > multi_packets.size())
        {
            // Destination buffer is full
            perror("recvfrom: Buffer full");
            return mPackets;
        }

        auto numberOfPackets = pos/TSPacket::TS_PACKET_SIZE;
        addAllPacketsAndResync(numberOfPackets, multi_packets);

        bytes_left = 0;
        // Notify that packets has been read
        mPartiallyRead.notify_one();
    }

    mDone = true;
    std::vector<TSPacketPtr> packets;
    {
        std::lock_guard< std::mutex > lock(mMutex);
        packets = mPackets;
    }
    return packets;
}


namespace {
uint32_t read_net_packets(
        int sock,
        unsigned char* dest_buf,
        size_t buf_size,
        fd_set read_set,
        struct sockaddr* addr,
        sigset_t* mask)
{
    socklen_t len;
    uint32_t num_bytes = 0;

    fd_set testset = read_set;

    int result = pselect(FD_SETSIZE, &testset, NULL, NULL, NULL, mask);
    if (result == 1 && FD_ISSET(sock, &testset))
    {
        ssize_t bytes;
        while ((bytes = recvfrom(sock,
                                 dest_buf + num_bytes,
                                 buf_size - num_bytes,
                                 0,
                                 addr,
                                 &len)) > 0)
        {
            num_bytes += bytes;
        }
        if (bytes < 0 && errno != EAGAIN)
        {
            // Failed to read but there are still data
            std::cout << "Fail to read socket\n";
        }
    }
    return num_bytes;
}

}