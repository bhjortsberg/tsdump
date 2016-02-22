#include <iostream>
#include "TransportStream.h"
#include "TSReport.h"
#include "PacketFilter.h"

using namespace std;

int main(int argc, char ** argv)
{
    PacketFilterPtr filter(new PacketFilter());
    std:string file_name;
    int ch;

    std::string sss("45,56,23,56,45");
    auto cc = std::count(std::begin(sss), std::end(sss), ',');
    std::cout << cc << std::endl;
    std::string fmt = "%u";
    for (int i = 0; i < cc; ++i)
    {
        fmt += ",%u";
    }

    auto it = std::begin(sss);
    do
    {
        std::cout << *it << " ";
        it = std::find(it, std::end(sss), ',');
    } while (it++ != std::end(sss));

//    return 0;


    while ((ch = getopt(argc, argv, "f:p:ter")) != -1) {
        std::string pids;
        int num_pids;
        switch (ch) {
            case 'f':
                file_name = std::string(optarg);
                break;
            case 'p':
                // Comma separated
                pids = std::string(optarg);
                num_pids = std::count(std::begin(pids), std::end(pids), ',');
//            std::search()

                break;
            case 't':
                filter->pts();
                break;
            case 'e':
                filter->ebp();
                break;
            case 'r':
                filter->rai();
                break;
//            default:
                break;
        }

    }
    TransportStream ts(file_name);
    TSReport report(ts, filter);

    report.report();

/*
    int ebp_cnt = 0, rai_cnt = 0;
    int i = 0;
    for (TSPacket packet : ts.getPackets()) {
        std::cout << i << " " << packet.pid();
        if (packet.has_pes_header())
            std::cout << " pts" << std::hex << " 0x" << packet.pes_header().get_pts() << std::dec << " " << packet.pes_header().get_pts_str();

        if (packet.has_adaption_field())
        {
            std::cout << "  adaption field";
            if (packet.has_random_access_indicator())
            {
                std::cout << " RAI";
                rai_cnt++;
            }


            if (packet.has_ebp())
            {
                std::cout << " EBP";
                ebp_cnt++;
            }
        }
        std::cout << std::endl;
        ++i;
    }
    std::cout << "=================================================" << std::endl;
    std::cout << "Number of RAI: " << rai_cnt << std::endl;
    std::cout << "Number of EBP: " << ebp_cnt << std::endl;
    std::cout << "Number of packets: " << i << std::endl;
*/

    return 0;
}