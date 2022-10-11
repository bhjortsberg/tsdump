//
// Created by Björn Hjortsberg on 2016-02-20.
//

#pragma once


#include <vector>

using Chunk = std::vector<unsigned char>;

class AdaptationField
{
public:
    AdaptationField(Chunk::const_iterator it);
    bool hasRandomAccessIndicator();
    bool hasEbp() const;
    unsigned int get_length_field();
    unsigned int size();
    std::string printStr();

private:
    unsigned int PCR_len() const;
    unsigned int OPCR_len() const;
    unsigned int splicing_point_len() const;
    unsigned int mLength;
    bool mDiscontinuityIndicator;
    bool mRandomAccessIndicator;
    bool mElementaryStreamPriorityIndicator;
    bool mPcrFlag;
    bool mOpcrFlag;
    bool mSplicingPointFlag;
    bool mTransportPrivateDataFlag;
    bool mAdaptationFieldExtensionFlag;
    Chunk mChunk;
};

