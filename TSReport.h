//
// Created by Björn Hjortsberg on 2016-02-22.
//

#pragma once

#include "TransportStream.h"
#include "IFilter.h"
#include "OutputOptions.h"

class TSReport
{
public:
    TSReport(TransportStream & tstream,
             const IFilterPtr & filter,
             const OutputOptionsPtr & option);
    void report();

private:
    std::string getPacketString(const TSPacketPtr & packet);
    std::string getPacketPayloadString(const TSPacketPtr & packet);
    std::string getPacketExtraInfoString(const TSPacketPtr & packet);
    void printPidInfo();
    void printHeader();
    void printSummary();
    IFilterPtr mFilter;
    OutputOptionsPtr mOption;
    TransportStream & mTransportStream;
    std::map<unsigned int, std::vector<int>> mContinuityError;
    unsigned int mPacketCount = 0;

};

