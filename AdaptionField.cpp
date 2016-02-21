//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <string>
#include "AdaptionField.h"

AdaptionField::AdaptionField(Chunk::const_iterator it)
{
    length = it[0];
    discontinuity_indicator =               it[1] & 0x80 ? true : false;
    random_access_indicator =               it[1] & 0x40 ? true : false;
    elementary_stream_priority_indicator =  it[1] & 0x20 ? true : false;
    PCR_flag =                              it[1] & 0x10 ? true : false;
    OPCR_flag =                             it[1] & 0x08 ? true : false;
    splicing_point_flag =                   it[1] & 0x04 ? true : false;
    transport_private_data_flag =           it[1] & 0x02 ? true : false;
    adaption_field_extension_flag =         it[1] & 0x01 ? true : false;

    chunk.resize(length-1); // The length in the adaption field does not calculate the length byte itself.
    std::copy(it+2, it+2+(length-1), std::begin(chunk));
}

bool AdaptionField::has_random_access_indicator()
{
    return random_access_indicator;
}


unsigned int AdaptionField::get_length_field()
{
    return length;
}

// The total  size of the adaption field
unsigned int AdaptionField::size()
{
    return length + 1;
}

bool AdaptionField::has_ebp() const
{
    if (transport_private_data_flag)
    {
        int private_data_pos = PCR_len() +
                               OPCR_len() +
                               splicing_point_len ();
        int trasport_private_data_len = chunk[private_data_pos];


        std::string str(std::begin(chunk)+private_data_pos+3, std::begin(chunk)+private_data_pos+6);
        if (chunk[private_data_pos+1] == 0xDF &&
                std::string(std::begin(chunk)+private_data_pos+3, std::begin(chunk)+private_data_pos+6) == "EBP")
        {
            return true;
        }
    }

    return false;
}

unsigned int AdaptionField::PCR_len() const
{
    if (PCR_flag)
        return 6;
    return 0;
}

unsigned int AdaptionField::OPCR_len() const
{
    if (OPCR_flag)
        return 6;
    return 0;
}

unsigned int AdaptionField::splicing_point_len() const
{
    if (splicing_point_flag)
        return 1;
    return 0;
}
