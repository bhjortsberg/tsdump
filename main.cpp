#include <iostream>
#include <sstream>
#include <unistd.h>
#include "TransportStream.h"
#include "TSReport.h"
#include "PacketFilter.h"

using namespace std;

void usage()
{
    std::cout << "tsparse [file] [options]" << std::endl;
    std::cout << "  Options:" << std::endl;
    std::cout << "          -f <file>           File that should be parsed" << std::endl;
    std::cout << "          -h                  help" << std::endl;
    std::cout << "          -t                  Print packets with pts" << std::endl;
    std::cout << "          -e                  Print packets with EBP markers" << std::endl;
    std::cout << "          -r                  Print packets with random access indicators" << std::endl;
    std::cout << "          -p <pid1,pid2...>   Print packets with pids, comma separated list of pids" << std::endl <<
                 "                              If no pids given, print a list of pids in transport stream" << std::endl;
    std::cout << "          -n <pkt num>        Print packet with given number" << std::endl;
    std::cout << "          -s                  Print only payload start packets" << std::endl;
}

int main(int argc, char ** argv)
{
    PacketFilterPtr filter(new PacketFilter());
    std:string file_name;
    int ch;

    if (argc < 2)
    {
        std::cerr << "Too few arguments" << std::endl;
        usage();
        return 0;
    }

    while ((ch = getopt(argc, argv, "n:hf:p:tesr")) != -1) {
        std::string pids_str;
        std::vector<int> pids;
        std::stringstream ss;

        switch (ch) {
            case 'n':
                filter->set_packet(std::atoi(optarg));
                break;
            case 'h':
                usage();
                break;
            case 'f':
                file_name = std::string(optarg);
                break;
            case 'p':
                ss << std::string(optarg);
                while (std::getline(ss, pids_str, ','))
                {
                    pids.push_back(std::stoi(pids_str));
                }
                filter->pids(pids);

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
            case 's':
                filter->payloadStart();
                break;
            default:
                usage();
                break;
        }

    }

    try {

        TransportStream ts(file_name);
        TSReport report(ts, filter);

        report.report();

    } catch (std::runtime_error & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
