//
// Created by Björn Hjortsberg on 2016-02-20.
//

#pragma once

#include <string>
#include <vector>

using Chunk = std::vector<unsigned char>;

class PESHeader
{
public:
    PESHeader(Chunk::const_iterator it);

    unsigned long long getPts();
    std::string getPtsStr();
    unsigned short getLength();
    std::string printStr();

private:
    unsigned short mLength;
    unsigned char mStreamId;
    unsigned char mPtsDtsFlags;
    char mPesHeaderDataLength;
    unsigned long long mPts;
    unsigned long long mDts;

};