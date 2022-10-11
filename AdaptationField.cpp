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
        mDiscontinuityIndicator = it[1] & 0x80 ? true : false;
        mRandomAccessIndicator = it[1] & 0x40 ? true : false;
        mElementaryStreamPriorityIndicator = it[1] & 0x20 ? true : false;
        mPcrFlag = it[1] & 0x10 ? true : false;
        mOpcrFlag = it[1] & 0x08 ? true : false;
        mSplicingPointFlag = it[1] & 0x04 ? true : false;
        mTransportPrivateDataFlag = it[1] & 0x02 ? true : false;
        mAdaptationFieldExtensionFlag = it[1] & 0x01 ? true : false;

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


unsigned int AdaptationField::get_length_field()
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
        int private_data_pos = PCR_len() +
                               OPCR_len() +
                               splicing_point_len ();
        int trasport_private_data_len = mChunk[private_data_pos];


        std::string str(std::begin(mChunk) + private_data_pos + 3, std::begin(mChunk) + private_data_pos + 6);
        if (mChunk[private_data_pos + 1] == 0xDF &&
            std::string(std::begin(mChunk) + private_data_pos + 3, std::begin(mChunk) + private_data_pos + 6) == "EBP")
        {
            return true;
        }
    }

    return false;
}

unsigned int AdaptationField::PCR_len() const
{
    if (mPcrFlag)
        return 6;
    return 0;
}

unsigned int AdaptationField::OPCR_len() const
{
    if (mOpcrFlag)
        return 6;
    return 0;
}

unsigned int AdaptationField::splicing_point_len() const
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
