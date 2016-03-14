//
// Created by Björn Hjortsberg on 20/02/16.
//

#include <stdexcept>
#include "TSPacket.h"

TSPacket::TSPacket(Chunk buffer, int pkt_num):
        chunk(buffer),
        m_pkt_num(pkt_num),
        m_prev(nullptr),
        m_next(nullptr)
{
    sync_byte = chunk[0];
    transport_error_indicator       = chunk[1] & 0x80 ? true : false;
    payload_unit_start_indicator    = chunk[1] & 0x40 ? true : false;
    transport_priority              = chunk[1] & 0x20 ? true : false;
    m_continuity_count                = chunk[3] & 0xf;

    // Initialize payload, pes and adaption field iterators
    payload_it = std::begin(chunk) + 4;
    pes_header_it = std::begin(chunk) + 4;
    adaption_field_it = std::begin(chunk) + 4;

    if (has_adaption_field()) {
        pes_header_it = adaption_field_it + adaption_field().size();
        payload_it = pes_header_it;
    }

    if (has_pes_header()) {
        payload_it += pes_header().get_length();
    }
}

unsigned short TSPacket::pid() const
{
    int pid = (chunk[1] & 0x1f) << 8;
    pid |= chunk[2];
    return pid;
}

unsigned short TSPacket::continuity_count() const
{
    return m_continuity_count;
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

Chunk TSPacket::get_payload() const {
    Chunk payload(TS_PACKET_SIZE);
//    std::copy(payload_it, std::end(chunk), std::begin(payload));
    std::copy(std::begin(chunk), std::end(chunk), std::begin(payload));
    return payload;
//    return PayloadIterator(payload_it, std::end(chunk));
}

bool TSPacket::is_payload_start() const
{
    return payload_unit_start_indicator;
}

int TSPacket::num() const
{
    return m_pkt_num;
}

void TSPacket::set_next(const TSPacketPtr & next)
{
    m_next = next;
}

void TSPacket::set_prev(const TSPacketPtr & prev)
{
    m_prev = prev;
}

bool TSPacket::continuity() const
{
    if (!m_prev)
        return true;

    return (m_prev->continuity_count() +1)%16 == m_continuity_count;
}

TSPacketPtr TSPacket::get_prev() const
{
    return m_prev;
}
