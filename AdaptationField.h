//
// Created by Björn Hjortsberg on 20/02/16.
//

#pragma once


#include <vector>

using Chunk = std::vector<unsigned char>;

class AdaptationField
{
public:
    AdaptationField(Chunk::const_iterator it);
    bool has_random_access_indicator();
    bool has_ebp() const;
    unsigned int get_length_field();
    unsigned int size();
    std::string print_str();

private:
    unsigned int PCR_len() const;
    unsigned int OPCR_len() const;
    unsigned int splicing_point_len() const;
    unsigned int length;
    bool discontinuity_indicator;
    bool random_access_indicator;
    bool elementary_stream_priority_indicator;
    bool PCR_flag;
    bool OPCR_flag;
    bool splicing_point_flag;
    bool transport_private_data_flag;
    bool adaptation_field_extension_flag;
    Chunk chunk;
};

