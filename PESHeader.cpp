//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <sstream>
#include "PESHeader.h"

PESHeader::PESHeader(Chunk::const_iterator it)
{
    stream_id = it[3];
    length = it[4];
    length |= (it[5] << 8);
    pts_dts_flags = (it[7] & 0xc0) >> 6;
    pes_header_data_length = it[8];

    if (pts_dts_flags == 0x02)
    {
        pts = static_cast<unsigned long>(it[13]) >> 1;          // bit 0-6
        pts |= static_cast<unsigned long>(it[12]) << 7;  // bit 7-14
        pts |= (static_cast<unsigned long>(it[11]) >> 1) << 15; // bit 15-21
        pts |= static_cast<unsigned long>(it[10]) << 22; // bit 22-29
        pts |= ((static_cast<unsigned long>(it[9]) & 0x0000000f) >> 1) << 30; // bit 30-32
    }

    if (pts_dts_flags == 0x03)
    {
        pts = static_cast<unsigned long>(it[13]) >> 1;          // bit 0-6
        pts |= static_cast<unsigned long>(it[12]) << 7;  // bit 7-14
        pts |= (static_cast<unsigned long>(it[11]) >> 1) << 15; // bit 15-21
        pts |= static_cast<unsigned long>(it[10]) << 22; // bit 22-29
        pts |= ((static_cast<unsigned long>(it[9]) & 0x0000000f) >> 1) << 30; // bit 30-32

        dts = static_cast<unsigned long>(it[18]) >> 1;          // bit 0-6
        dts |= static_cast<unsigned long>(it[17]) << 7;  // bit 7-14
        dts |= (static_cast<unsigned long>(it[16]) >> 1) << 15; // bit 15-21
        dts |= static_cast<unsigned long>(it[15]) << 22; // bit 22-29
        dts |= ((static_cast<unsigned long>(it[14]) & 0x0000000f) >> 1) << 30; // bit 30-32

    }


}

unsigned long long PESHeader::get_pts()
{
    return pts;
}

std::string PESHeader::get_pts_str()
{
    std::stringstream ss;

    unsigned long millisec = pts/90;
    int hour = millisec/(1000*60*60);
    millisec -= hour*1000*60*60;
    int minutes = millisec/(1000*60);
    millisec -= minutes*1000*60;
    int seconds = millisec/1000;
    millisec -= seconds*1000;

    ss << hour << ":" << minutes << ":" << seconds << "." << millisec;
    return ss.str();
}

unsigned short PESHeader::get_length() {
    return length;
}
