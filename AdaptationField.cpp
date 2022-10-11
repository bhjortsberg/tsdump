//
// Created by Björn Hjortsberg on 2016-02-20.
//

#include <string>
#include <ostream>
#include <sstream>
#include "AdaptationField.h"

AdaptationField::AdaptationField(Chunk::const_iterator it)
{
    mLength = it[0];
    if (mLength > 0)
    {
        mDiscontinuityIndicator = (it[1] & 0x80) != 0;
        mRandomAccessIndicator = (it[1] & 0x40) != 0;
        mElementaryStreamPriorityIndicator = (it[1] & 0x20) != 0;
        mPcrFlag = (it[1] & 0x10) != 0;
        mOpcrFlag = (it[1] & 0x08) != 0;
        mSplicingPointFlag = (it[1] & 0x04) != 0;
        mTransportPrivateDataFlag = (it[1] & 0x02) != 0;
        mAdaptationFieldExtensionFlag = (it[1] & 0x01) != 0;

        mChunk.resize(mLength - 1); // The mLength in the adaptation field does not calculate the mLength byte itself.
        std::copy(it + 2, it + 2 + (mLength - 1), std::begin(mChunk));
    } else {
        mDiscontinuityIndicator =               false;
        mRandomAccessIndicator =               false;
        mElementaryStreamPriorityIndicator =  false;
        mPcrFlag =                              false;
        mOpcrFlag =                             false;
        mSplicingPointFlag =                   false;
        mTransportPrivateDataFlag =           false;
        mAdaptationFieldExtensionFlag =         false;

    }

}

bool AdaptationField::hasRandomAccessIndicator()
{
    return mRandomAccessIndicator;
}


unsigned int AdaptationField::getLengthField()
{
    return mLength;
}

// The total  size of the adaptation field
unsigned int AdaptationField::size()
{
    return mLength + 1;
}

bool AdaptationField::hasEbp() const
{
    if (mTransportPrivateDataFlag)
    {
        int privateDataPosition = pcrLength() + oPcrLength() + splicingPointLen();
        int trasportPrivateDataLength = mChunk[privateDataPosition];

        std::string str(std::begin(mChunk) + privateDataPosition + 3, std::begin(mChunk) + privateDataPosition + 6);
        if (mChunk[privateDataPosition + 1] == 0xDF &&
            std::string(std::begin(mChunk) + privateDataPosition + 3, std::begin(mChunk) + privateDataPosition + 6) == "EBP")
        {
            return true;
        }
    }

    return false;
}

unsigned int AdaptationField::pcrLength() const
{
    if (mPcrFlag)
        return 6;
    return 0;
}

unsigned int AdaptationField::oPcrLength() const
{
    if (mOpcrFlag)
        return 6;
    return 0;
}

unsigned int AdaptationField::splicingPointLen() const
{
    if (mSplicingPointFlag)
        return 1;
    return 0;
}

std::string AdaptationField::printStr()
{
    std::stringstream print_str;

    print_str << "Adaptation Field" << std::endl;
    print_str <<
              "\tdiscontinuity            : " << mDiscontinuityIndicator << std::endl <<
              "\trandom access indicator  : " << mRandomAccessIndicator << std::endl <<
              "\tpriority indicator       : " << mElementaryStreamPriorityIndicator << std::endl <<
              "\tPCR_flag                 : " << mPcrFlag << std::endl <<
              "\tOPCR_flag                : " << mOpcrFlag << std::endl <<
              "\tsplicing point           : " << mSplicingPointFlag << std::endl <<
              "\ttransport private data   : " << mTransportPrivateDataFlag << std::endl <<
              "\textension flag           : " << mAdaptationFieldExtensionFlag << std::endl << std::endl;

    return print_str.str();
}