//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <stdexcept>
#include "TSPacket.h"

TSPacket::TSPacket(Chunk buffer): chunk(buffer)
{
    if (has_adaption_field()) {
        adaption_field_it = std::begin(chunk) + 4;
        pes_header_it = adaption_field_it + adaption_field().size();
    }
    else {
        pes_header_it = std::begin(chunk) + 4;
    }

    payload_it = pes_header_it;
    if (has_pes_header()) {
        payload_it += pes_header().get_length();
    }
}

unsigned short TSPacket::pid() const
{
    int pid = (chunk[1] & 0x0f) << 8;
    pid |= chunk[2];
    return pid;
}

bool TSPacket::has_adaption_field() const
{
    return adaption_field_control() & 0x02 ? true : false;
}

char TSPacket::adaption_field_control() const
{
    return (chunk[3] & 0x30) >> 4;
}

bool TSPacket::has_random_access_indicator() const
{
    return has_adaption_field() ? adaption_field().has_random_access_indicator() : false;
}

AdaptionField TSPacket::adaption_field() const
{
    if (has_adaption_field())
    {
        return AdaptionField(std::begin(chunk)+4);
//        return AdaptionField(adaption_field_it);
    }

    throw std::runtime_error("No adaption field");

}

PESHeader TSPacket::pes_header() const
{
    if (has_pes_header())
    {
        int pes_pos = 4;

        if (has_adaption_field())
        {
            pes_pos += adaption_field().size();
        }

        return PESHeader(std::begin(chunk) + pes_pos);
    }

    throw std::runtime_error("No PES Header");
}

bool TSPacket::has_pes_header() const
{
    int pes_pos = 4;
    if (has_adaption_field()) {
        pes_pos += adaption_field().size();
    }

    if (chunk[pes_pos] == 0x00 &&
            chunk[pes_pos+1] == 0x00 &&
            chunk[pes_pos+2] == 0x01) {
        return true;
    }

    return false;
}

bool TSPacket::has_ebp() const
{
    return has_adaption_field() && adaption_field().has_ebp();
}

PayloadIterator TSPacket::get_payload() const {
    return PayloadIterator(payload_it, std::end(chunk));
}
