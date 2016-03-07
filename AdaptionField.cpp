//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <string>
#include <ostream>
#include <sstream>
#include "AdaptionField.h"

AdaptionField::AdaptionField(Chunk::const_iterator it)
{
    length = it[0];
    if (length > 0)
    {
        discontinuity_indicator =               it[1] & 0x80 ? true : false;
        random_access_indicator =               it[1] & 0x40 ? true : false;
        elementary_stream_priority_indicator =  it[1] & 0x20 ? true : false;
        PCR_flag =                              it[1] & 0x10 ? true : false;
        OPCR_flag =                             it[1] & 0x08 ? true : false;
        splicing_point_flag =                   it[1] & 0x04 ? true : false;
        transport_private_data_flag =           it[1] & 0x02 ? true : false;
        adaption_field_extension_flag =         it[1] & 0x01 ? true : false;

        chunk.resize(length - 1); // The length in the adaption field does not calculate the length byte itself.
        std::copy(it + 2, it + 2 + (length - 1), std::begin(chunk));
    } else {
        discontinuity_indicator =               false;
        random_access_indicator =               false;
        elementary_stream_priority_indicator =  false;
        PCR_flag =                              false;
        OPCR_flag =                             false;
        splicing_point_flag =                   false;
        transport_private_data_flag =           false;
        adaption_field_extension_flag =         false;

    }

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

std::string AdaptionField::print_str()
{
    std::stringstream print_str;

    print_str << "Adaption Field" << std::endl;
    print_str <<
            "\tdiscontinuity            : " << discontinuity_indicator << std::endl <<
            "\trandom access indicator  : " << random_access_indicator << std::endl <<
            "\tpriority indicator       : " << elementary_stream_priority_indicator << std::endl <<
            "\tPCR_flag                 : " << PCR_flag << std::endl <<
            "\tOPCR_flag                : " << OPCR_flag << std::endl <<
            "\tsplicing point           : " << splicing_point_flag << std::endl <<
            "\ttransport private data   : " << transport_private_data_flag << std::endl <<
            "\textension flag           : " << adaption_field_extension_flag << std::endl << std::endl;

    return print_str.str();
}
