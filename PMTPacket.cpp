//
// Created by Björn Hjortsberg on 2016-04-20.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include "PMTPacket.h"
#include "TSPacket.h"


const std::string streamTypeArr[] = {
        "ITU-T | ISO/IEC Reserved",
        "ISO/IEC 11172-2 (MPEG-1 video)",
        "ITU-T Rec. H.262 and ISO/IEC 13818-2 (MPEG-2 higher rate interlaced video)",
        "ISO/IEC 11172-3 (MPEG-1 audio)",
        "ISO/IEC 13818-3 (MPEG-2 halved sample rate audio)",
        "ITU-T Rec. H.222 and ISO/IEC 13818-1 (MPEG-2 tabled data)",
        "ITU-T Rec. H.222 and ISO/IEC 13818-1 (MPEG-2 packetized data)",
        "ISO/IEC 13522 (MHEG)",
        "ITU-T Rec. H.222 and ISO/IEC 13818-1 DSM CC",
        "ITU-T Rec. H.222 and ISO/IEC 13818-1/11172-1 auxiliary data",
        "ISO/IEC 13818-6 DSM CC multiprotocol encapsulation",
        "ISO/IEC 13818-6 DSM CC U-N messages",
        "ISO/IEC 13818-6 DSM CC stream descriptors",
        "ISO/IEC 13818-6 DSM CC tabled data",
        "ISO/IEC 13818-1 auxiliary data",
        "ISO/IEC 13818-7 ADTS AAC (MPEG-2 lower bit-rate audio)",
        "ISO/IEC 14496-2 (MPEG-4 H.263 based video)",
        "\tISO/IEC 14496-3 (MPEG-4 LOAS multi-format framed audio)",
        "ISO/IEC 14496-1 (MPEG-4 FlexMux)",
        "\tISO/IEC 14496-1 (MPEG-4 FlexMux)",
        "ISO/IEC 13818-6 DSM CC synchronized download protocol",
        "Packetized metadata",
        "Sectioned metadata",
        "ISO/IEC 13818-6 DSM CC Data Carousel metadata",
        "ISO/IEC 13818-6 DSM CC Object Carousel metadata",
        "\tISO/IEC 13818-6 Synchronized Download Protocol metadata",
        "ISO/IEC 13818-11 IPMP",
        "ITU-T Rec. H.264 and ISO/IEC 14496-10 (lower bit-rate video)",
        "ISO/IEC 14496-3 (MPEG-4 raw audio)",
        "ISO/IEC 14496-17 (MPEG-4 text)",
        "ISO/IEC 23002-3 (MPEG-4 auxiliary video)",
        "ISO/IEC 14496-10 SVC (MPEG-4 AVC sub-bitstream)",
        "ISO/IEC 14496-10 MVC (MPEG-4 AVC sub-bitstream)",
        "ITU-T Rec. T.800 and ISO/IEC 15444 (JPEG 2000 video)",
        "Reserved",                                                 // 34
        "Reserved",                                                 // 35
        "ITU-T Rec. H.265 and ISO/IEC 23008-2 (Ultra HD video)",    // 36
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",                 // 50
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",                                     // 65
        "Chinese Video Standard",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",     // 100
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",     // 120
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "ISO/IEC 13818-11 IPMP (DRM)",     // 127
        "ITU-T Rec. H.262 and ISO/IEC 13818-2 with DES-64-CBC encryption for DigiCipher II or PCM audio for Blu-ray",
        "Dolby Digital (AC-3) up to six channel audio for ATSC and Blu-ray",
        "SCTE subtitle or DTS 6 channel audio for Blu-ray",
        "Dolby TrueHD lossless audio for Blu-ray",
        "Dolby Digital Plus (enhanced AC-3) up to 16 channel audio for Blu-ray",
        "DTS 8 channel audio for Blu-ray",
        "SCTE-35[5] digital program insertion cue message or DTS 8 channel lossless audio for Blu-ray",
        "Dolby Digital Plus (enhanced AC-3) up to 16 channel audio for ATSC",
        "Privately defined",    // 136
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Blu-ray Presentation Graphic Stream (subtitling)",
        "ATSC DSM CC Network Resources table",
        "Privately defined",    // 146
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",    // 180
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",
        "Privately defined",    // 191
        "DigiCipher II text",
        "Dolby Digital (AC-3) up to six channel audio with AES-128-CBC data encryption",
        "ATSC DSM CC synchronous data or Dolby Digital Plus up to 16 channel audio with AES-128-CBC data encryption",
        "Privately defined."    // 195
        "Privately defined."    // 196
        "Privately defined."    // 197
        "Privately defined."    // 198
        "Privately defined."    // 199
        "Privately defined."    // 200
        "Privately defined."    // 201
        "Privately defined."    // 202
        "Privately defined."    // 203
        "Privately defined."    // 204
        "Privately defined."    // 205
        "Privately defined."    // 206
        "ISO/IEC 13818-7 ADTS AAC with AES-128-CBC frame encryption",
        "Privately defined.",
        "BBC Dirac (Ultra HD video)",
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 210
        "Privately defined.",   // 218
        "ITU-T Rec. H.264 and ISO/IEC 14496-10 with AES-128-CBC slice encryption",
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Privately defined.",   // 220
        "Microsoft Windows Media Video 9 (lower bit-rate video)",   // 234
        "Privately defined.",   // 235
        "Privately defined.",   // 236
        "Privately defined.",   // 237
        "Privately defined.",   // 238
        "Privately defined.",   // 239
        "Privately defined.",   // 240
        "Privately defined.",   // 241
        "Privately defined.",   // 242
        "Privately defined.",   // 243
        "Privately defined.",   // 244
        "Privately defined.",   // 245
        "Privately defined.",   // 246
        "Privately defined.",   // 247
        "Privately defined.",   // 248
        "Privately defined.",   // 249
        "Privately defined.",   // 250
        "Privately defined.",   // 251
        "Privately defined.",   // 252
        "Privately defined.",   // 253
        "Privately defined.",   // 254
        "Privately defined.",   // 255
};

const std::string streamTypeReserved = "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved";
const std::string streamTypeUserPrivate = "User Private";

PMTPacket::PMTPacket(const TSPacketPtr &packet):
        mThis(packet)
{

}

std::vector< int > PMTPacket::getElementaryPids() const
{
    std::vector<int> pids;

    std::for_each(std::begin(mElementaryStreams), std::end(mElementaryStreams),
                  [&pids](std::pair< unsigned int, unsigned char> thing){
                      pids.emplace_back(thing.first);
                  });
    return pids;
}

bool PMTPacket::isPmt() const
{
    // section_number and last_section_number should be zero
    if (*(mThis->payload() + 7) == 0 && *(mThis->payload() + 8) == 0)
    {
        return true;
    }
    return false;
}

unsigned int PMTPacket::programInfoLength() const
{
    unsigned short programInfoLength = 0;

    programInfoLength |= (*(mThis->payload() + 11) & 0x0F) << 8;
    programInfoLength |= *(mThis->payload() + 12);

    return programInfoLength;
}

std::string PMTPacket::streamTypeString(unsigned int pid) const
{
    std::string typeString;
    auto type = streamType(pid);

    if (type < 0x29) {
        typeString = streamTypeArr[type];
    } else if (type < 0x80) {
        typeString = streamTypeReserved;
    } else {
        typeString = streamTypeUserPrivate;
    }

    return typeString;
}

unsigned char PMTPacket::streamType(unsigned int pid) const
{
    auto it = mElementaryStreams.find(pid);
    if (it != std::end(mElementaryStreams))
    {
        return it->second;
    }
    return 0;
}

void PMTPacket::parse()
{

    unsigned short section_length;
    section_length = (*(mThis->payload() + 2) & 0x0f) << 8;
    section_length |= *(mThis->payload() + 3);

    mProgramInfoLength = programInfoLength();
    auto pit = mThis->payload() + 13 + mProgramInfoLength;

    unsigned short elementaryStreamInfoLength = 0;

    int bytes_left2 = section_length - 13 - mProgramInfoLength;
    for (int bytes_left = section_length - 13 - mProgramInfoLength; bytes_left > 0; bytes_left -= 5 + elementaryStreamInfoLength)
    {
        unsigned short elementary_pid = 0;
        unsigned char stream_type = *(pit++);

        elementary_pid |= (*(pit++) & 0x1F) << 8;
        elementary_pid |= *(pit++);

        elementaryStreamInfoLength = (*(pit++) & 0x0F) << 8;
        elementaryStreamInfoLength |= *(pit++);

        pit += elementaryStreamInfoLength;
        mElementaryStreams.insert({elementary_pid, stream_type});
    }

}

Chunk::const_iterator PMTPacket::payload() const
{
    return mThis->payload();
}


PMTPacket parse_pmt(const TSPacketPtr & packet)
{
    PMTPacket pmt(packet);

    if (!pmt.isPmt()) {
        throw std::runtime_error("Packet not a PMT");
    }

    if (pmt.programInfoLength() != 0) {
        throw std::runtime_error("Program info length not 0");
    }

    pmt.parse();

    return pmt;
}


