//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <sstream>
#include <iomanip>
#include "PESHeader.h"

PESHeader::PESHeader(Chunk::const_iterator it)
{
    mStreamId = it[3];
    mLength = it[4] << 8;
    mLength |= it[5];
    mPtsDtsFlags = (it[7] & 0xc0) >> 6;
    mPesHeaderDataLength = it[8];

    if (mPtsDtsFlags == 0x02)
    {
        mPts = static_cast<unsigned long>(it[13]) >> 1;          // bit 0-6
        mPts |= static_cast<unsigned long>(it[12]) << 7;  // bit 7-14
        mPts |= (static_cast<unsigned long>(it[11]) >> 1) << 15; // bit 15-21
        mPts |= static_cast<unsigned long>(it[10]) << 22; // bit 22-29
        mPts |= ((static_cast<unsigned long>(it[9]) & 0x0000000f) >> 1) << 30; // bit 30-32
    }

    if (mPtsDtsFlags == 0x03)
    {
        mPts = static_cast<unsigned long>(it[13]) >> 1;          // bit 0-6
        mPts |= static_cast<unsigned long>(it[12]) << 7;  // bit 7-14
        mPts |= (static_cast<unsigned long>(it[11]) >> 1) << 15; // bit 15-21
        mPts |= static_cast<unsigned long>(it[10]) << 22; // bit 22-29
        mPts |= ((static_cast<unsigned long>(it[9]) & 0x0000000f) >> 1) << 30; // bit 30-32

        mDts = static_cast<unsigned long>(it[18]) >> 1;          // bit 0-6
        mDts |= static_cast<unsigned long>(it[17]) << 7;  // bit 7-14
        mDts |= (static_cast<unsigned long>(it[16]) >> 1) << 15; // bit 15-21
        mDts |= static_cast<unsigned long>(it[15]) << 22; // bit 22-29
        mDts |= ((static_cast<unsigned long>(it[14]) & 0x0000000f) >> 1) << 30; // bit 30-32

    }

}

unsigned long long PESHeader::getPts()
{
    return mPts;
}

std::string PESHeader::getPtsStr()
{
    std::stringstream ss;

    unsigned long millisec = mPts / 90;
    int hour = millisec/(1000*60*60);
    millisec -= hour*1000*60*60;
    int minutes = millisec/(1000*60);
    millisec -= minutes*1000*60;
    int seconds = millisec/1000;
    millisec -= seconds*1000;

    ss << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << "."
        << std::setfill('0') << std::setw(3) << millisec;
    return ss.str();
}

unsigned short PESHeader::getLength() {
    return mLength;
}

std::string PESHeader::printStr()
{
    std::stringstream printStr;
    printStr << "PES header:" << std::endl;
    printStr << "\tstream_id : " << static_cast<int>(mStreamId) << std::endl <<
             "\tlength : " << mLength << std::endl <<
             "\tpes_header_length : " << static_cast<int>(mPesHeaderDataLength) << std::endl;
    printStr << "\tpts_dts_flag : 0x" << std::hex << static_cast<int>(mPtsDtsFlags) << std::endl;

    if (mPtsDtsFlags == 0x02)
    {
        printStr << "\t\tpts : " << getPtsStr() << std::endl;
    }
    if (mPtsDtsFlags == 0x03)
    {
        printStr << "\t\tpts : " << getPtsStr() << std::endl;
        printStr << "\t\tdts : " << "TBD" << std::endl;
    }

    return printStr.str();
}