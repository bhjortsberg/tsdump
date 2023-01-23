//
// Created by Björn Hjortsberg on 20/02/16.
//

#ifndef TSPARSER_TSPACKET_H
#define TSPARSER_TSPACKET_H

#include <vector>
#include <map>
#include <memory>
#include "AdaptationField.h"
#include "PESHeader.h"
#include "IPacket.h"


uint32_t find_synch_byte(const std::vector<uint8_t>::iterator& src_packets, uint32_t size);

typedef std::pair<Chunk::const_iterator, Chunk::const_iterator> PayloadIterator;

class TSPacket;
typedef std::shared_ptr<TSPacket> TSPacketPtr;

class TSPacket : public IPacket
{
public:
    static const int SYNC_BYTE = 0x47;
    static const int TS_PACKET_SIZE = 188;

    TSPacket(Chunk buffer, int pkt_num);
    unsigned short pid() const;
    unsigned short continuity_count() const;
    bool has_adaptation_field() const;
    bool has_ebp() const;
    bool has_random_access_indicator() const;
    AdaptationField adaptation_field() const;
    bool has_pes_header() const;
    PESHeader pes_header() const;
    Chunk get_payload() const;
    bool is_payload_start() const;
    int num() const;
    void set_next(const TSPacketPtr & next);
    void set_prev(const TSPacketPtr & prev);
    bool continuity() const;
    TSPacketPtr get_prev() const;
    std::map<unsigned short, unsigned short> get_program_pids() const;
    std::vector<int> parse_pmt() const;

    virtual Chunk::const_iterator payload() const;
    const uint8_t * raw() const;
    size_t size() const;

private:
    char adaptation_field_control() const;
    Chunk chunk;
    char sync_byte;
    bool transport_error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    unsigned short m_continuity_count;
    int m_pkt_num;

    TSPacketPtr m_prev;
    TSPacketPtr m_next;

    // TODO: Iterator for adaptation field, pes header that is set to the position of them
    // and used by all functions instead of code duplication like now.
    // Partly done. One have to set them to std::begin(chunk) and compare to that.
    Chunk::iterator pes_header_it;
    Chunk::iterator adaptation_field_it;
    Chunk::const_iterator payload_it;
};


#endif //TSPARSER_TSPACKET_H
