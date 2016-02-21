//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TSPACKET_H
#define TSPARSER_TSPACKET_H

#include <vector>
#include "AdaptionField.h"
#include "PESHeader.h"

typedef std::vector<unsigned char> Chunk;

class TSPacket
{
public:
    static const int SYNC_BYTE = 0x47;
    static const int TS_PACKET_SIZE = 188;

    TSPacket(Chunk buffer);
    unsigned short pid();
    bool has_adaption_field() const;
    bool has_ebp() const;
    bool has_random_access_indicator() const;
    AdaptionField adaption_field() const;
    bool has_pes_header() const;
    PESHeader pes_header() const;

private:
    char adaption_field_control() const;
    Chunk chunk;

    // TODO: Iterator for adaption field, pes header that is set to the position of them
    // and used by all functions instead of code duplication like now.
    // Partly done. One have to set them to std::begin(chunk) and compare to that.
    Chunk::iterator pes_header_it;
    Chunk::iterator adaption_field_it;
};


#endif //TSPARSER_TSPACKET_H
