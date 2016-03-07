//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_PESHEADER_H
#define TSPARSER_PESHEADER_H

#include <string>
#include <vector>

typedef std::vector<unsigned char> Chunk;

class PESHeader
{
public:
    PESHeader(Chunk::const_iterator it);

    unsigned long long get_pts();
    std::string get_pts_str();
    unsigned short get_length();
    std::string print_str();

private:
    unsigned short length;
    unsigned char stream_id;
    unsigned char pts_dts_flags;
    char pes_header_data_length;
    unsigned long long pts;
    unsigned long long dts;

};


#endif //TSPARSER_PESHEADER_H
