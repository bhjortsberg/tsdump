//
// Created by Björn Hjortsberg on 2016-02-22.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include "TSReport.h"

TSReport::TSReport(TransportStream &tstream,
                   const IFilterPtr & filter,
                   const OutputOptionsPtr & option):
        mTransportStream(tstream),
        mFilter(filter),
        mOption(option)
{

}

void writePacket(const TSPacketPtr& packet, std::ofstream& stream)
{
    const char * packet_data = reinterpret_cast<const char*>(packet->raw());
    stream.write(packet_data, packet->size());
}

void TSReport::report()
{
    if (mOption->listPidsOnly()) {
        printPidInfo();
        mTransportStream.stop();
        mTransportStream.getPackets();
        return;
    }

    mPacketCount = 0;
    printHeader();
    bool doLoop = true;
    while (doLoop)
    {
        for (const auto& packet : mTransportStream.getPackets())
        {
            ++mPacketCount;
            if (mFilter->show(packet))
            {
                std::cout << getPacketString(packet);

                if (mOption->printExtraInfo())
                {
                    std::cout << getPacketExtraInfoString(packet) << "\n";
                }
                if (mOption->printPayload())
                {
                    std::cout << getPacketPayloadString(packet) << "\n";
                }

                if (!packet->continuity())
                {
                    auto it = mContinuityError.find(packet->pid());
                    if (it != std::end(mContinuityError))
                    {
                        it->second.push_back(packet->number());
                        mContinuityError.emplace(packet->pid(), it->second);
                    }
                    else
                    {
                        std::vector<int> v {packet->number()};
                        mContinuityError.emplace(packet->pid(), v);
                    }
                }
                if (mOption->fileOutput())
                {
                    writePacket(packet, mOption->outputFile());
                }
            }
            if (mTransportStream.isStopped())
            {
                doLoop = false;
                break;
            }
        }
        if (mTransportStream.isDone())
        {
            break;
        }
    }

    printSummary();

//    std::cout << mFilter->statistics();

}

std::string TSReport::getPacketString(const TSPacketPtr & packet)
{
    std::stringstream packetString;

    packetString << packet->number() << "\t" << packet->pid() << "\t" << packet->continuityCount() << "\t\t";
    if (packet->hasPesHeader())
    {
        packetString << std::hex << "0x"
                      << packet->pesHeader().getPts()
                      << std::dec << "\t"
                      << packet->pesHeader().getPtsStr() << "\t"
                      << packet->pesHeader().getLength() << "\t";
    }
    else {
        packetString << "\t\t\t\t";
    }


    if (packet->hasAdaptationField() && packet->hasEbp())
    {
        packetString << "EBP\t";
    }
    else {
        packetString << "   \t";
    }

    if (packet->hasRandomAccessIndicator())
    {
        packetString << "RAI\t";
    }
    else {
        packetString << "   \t";
    }

    if (packet->isPayloadStart())
    {
        packetString << "Start\t";
    }

    if (packet->hasPesHeader())
    {
        TSPacketPtr prev_pes_packet = findPrevPesPacket(packet);
        if (prev_pes_packet)
        {
            int pts_diff = packet->pesHeader().getPts() - prev_pes_packet->pesHeader().getPts();
            if (pts_diff > 20000)
            {
                packetString << "PTS diff: " << pts_diff;
            }
        }
    }

    packetString << "\n";

    return packetString.str();

}

void TSReport::printHeader()
{
    std::cout << "Packet\tPID\tContinuity\tpts hex\t\tpts wall\tSize\tEBP\tRAI\tPayload\n";
    std::cout << "-----------------------------------------------------------------------------------------------\n";
}

std::string TSReport::getPacketPayloadString(const TSPacketPtr &packet)
{
    std::stringstream payloadStr;
    payloadStr << std::hex << "\n\t\t";

    int byteCount = 0;
    std::stringstream line;
    std::string asciiline = "";
    for (auto & p : packet->getPayload())
    {
        line << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(p) << " ";
        asciiline += (((p > 31) && (p < 127)) ? p : '.');

        ++byteCount;

        if (byteCount % 8 == 0) {
            line << " ";
            asciiline += " ";
        }
        if (byteCount % 16 == 0) {
            payloadStr << line.str() << " | " << asciiline << "\n\t\t";
            line.str("");
            asciiline = "";
        }
    }
    payloadStr << std::left << std::setfill(' ') << std::setw(16 * 3 + 2) << line.str() << " | " << asciiline << "\n";

    return payloadStr.str();
}

std::string TSReport::getPacketExtraInfoString(const TSPacketPtr &packet)
{
    std::stringstream adaptationStr;
    std::stringstream pesStr;
    pesStr << "\n";

    if (packet->hasAdaptationField())
    {
        adaptationStr << "\n" << packet->adaptationField().printStr();
    }

    if (packet->hasPesHeader())
    {
        pesStr << packet->pesHeader().printStr();
    }

    return adaptationStr.str() + pesStr.str();

}

void TSReport::printSummary()
{
    std::cout << std::endl << "Summary:" << std::endl;
    std::cout << "\tNumber of packets: " << mPacketCount << std::endl;

    if (!mContinuityError.empty())
    {
        std::cout << "\tContinuity count error in:" << std::endl;
        for (const auto & a : mContinuityError)
        {
            bool first = true;
            std::cout << "\t\tpid: " << a.first << " packet numbers: ";
            for (const auto & b : a.second)
            {
                std::cout << (first ? std::to_string(b) : ", " + std::to_string(b));
                first = false;
            }
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
}

TSPacketPtr TSReport::findPrevPesPacket(const TSPacketPtr & packet)
{
    TSPacketPtr currentPacket = packet;

    while ((currentPacket = currentPacket->getPreviousPacket()) != nullptr)
    {
        if (currentPacket->hasPesHeader())
        {
            break;
        }
    }

    return currentPacket;
}

void TSReport::printPidInfo()
{
    std::cout << std::setw(6) << "Pid\tType" << std::endl;
    std::cout << "=====================" << std::endl;
    TSPacketPtr pat;
    while (!pat) {
        pat = mTransportStream.findPat();
        if (mTransportStream.isStopped()) {
            return;
        }
    }

    auto pmtPids = mTransportStream.findPmtPids(pat);
    for (auto p : pmtPids) {
        std::cout << getPmtString(p);
    }

    auto pmts = mTransportStream.getPmts(pat);
    for (auto p : pmts) {
        std::cout << getEsString(p) << std::endl;
    }
}

std::string TSReport::getPmtString(unsigned int pid)
{
    std::stringstream ss;
    ss << std::setw(4) << std::right << std::dec << std::setfill(' ') << pid << "\t";
    ss << std::left << "PMT " << std::endl;

    return ss.str();
}

std::string TSReport::getEsString(const PMTPacket &pmtPacket)
{
    std::stringstream ss;
    for (auto p : pmtPacket.getElementaryPids())
    {
        ss << std::setw(4) << std::right << std::dec << std::setfill(' ') << p << "\t";
        ss << "0x" << std::setw(2) << std::hex << std::setfill('0') << static_cast<int>(pmtPacket.streamType(p));
        ss << std::left << " " << pmtPacket.streamTypeString(p) << std::endl;
    }

    return ss.str();
}
