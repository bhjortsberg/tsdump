#include <iostream>
#include <sstream>
#include <unistd.h>
#include <array>
#include "TransportStream.h"
#include "TSReport.h"
#include "PacketFilter.h"
#include "OutputOptions.h"

using namespace std;

void usage();
std::vector<int> get_opt_values(char * opt);
std::vector<std::string> get_opt_comma_separated(const char * opt);
std::vector<int> get_opt_range(const std::string & str);

int main(int argc, char ** argv)
{
    PacketFilterPtr filter(new PacketFilter());
    OutputOptionsPtr option(new OutputOptions());
    std:string file_name;
    int ch;

    if (argc < 2)
    {
        std::cerr << "Too few arguments" << std::endl;
        usage();
        return 0;
    }

    while ((ch = getopt(argc, argv, "i:hf:p:tesrXx")) != -1) {
        std::string pids_str, pkt_str;
        std::vector<int> pids, pkts;
        std::stringstream ss, pss, ppss;
        std::vector<std::string> ivec;
        std::array<int,2> range;
        int i;

        switch (ch) {
            case 'i':
                filter->packets(get_opt_values(optarg));
#if 0
                pss << std::string(optarg);

                while (std::getline(pss, pkt_str, ','))
                {
                    ivec.push_back(pkt_str);
//                    pkts.push_back(std::stoi(pkt_str));
                }
                for (auto a : ivec)
                {
                    ppss << a;
                    i =0 ;
                    while (std::getline(ppss, pkt_str, '-'))
                    {
                        range[i] = std::stoi(pkt_str);
                        ++i;
                    }

                    for(int j = range[0]; j <= range[1]; ++j)
                    {
                        pkts.push_back(j);
                    }
//                    find(std::begin(a), std::end(a), '-');
                    // copy_if(std::begin(a), std::end(a), std::begin(res), [bool](const auto & g){});
                    // transform(std::begin(a), std::end(a), std::begin(res), std::stoi);
                    // generate(std::begin(arr), std::end(arr),  <- kan man generera en range?
                }
                filter->packets(pkts);
#endif
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
            case 'x':
                option->extraInfo();
                break;
            case 'X':
                option->payload();
                break;
            default:
                usage();
                break;
        }

    }

    if (argv[optind] != 0)
    {
        file_name = std::string(argv[optind]);
    }

    try {

        TransportStream ts(file_name);
        TSReport report(ts, filter, option);

        report.report();

    } catch (std::runtime_error & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


std::vector<int> get_opt_values(char * opt)
{
    std::vector<std::string> ivec;
    std::vector<int> packets;

    ivec = get_opt_comma_separated(opt);

    for_each(std::begin(ivec), std::end(ivec), get_opt_range);

    for (auto a : ivec)
    {
        auto p = get_opt_range(a);
        std::copy(std::begin(p), std::end(p), std::end(packets));
#if 0
        pss2 << a;
        int i = 0 ;
        std::string newstr;
        while (std::getline(pss2, newstr, '-'))
        {
            range[i] = std::stoi(newstr);
            ++i;
        }
        if (i != 0)
        {
            for (int j = range[0]; j <= range[1]; ++j)
            {
                packets.push_back(j);
            }
        }
        else
        {
            packets.push_back(std::stoi(a));
        }
#endif
//       find(std::begin(a), std::end(a), '-');
        // copy_if(std::begin(a), std::end(a), std::begin(res), [bool](const auto & g){});
        // transform(std::begin(a), std::end(a), std::begin(res), std::stoi);
        // generate(std::begin(arr), std::end(arr),  <- kan man generera en range?
    }

    return packets;
}

std::vector<std::string> get_opt_comma_separated(const char * opt)
{
    std::stringstream pss;
    std::string pkt_str;
    std::vector<std::string> vec;
    pss << std::string(opt);

    while (std::getline(pss, pkt_str, ','))
    {
        vec.push_back(pkt_str);
    }

    return vec;
}
std::vector<int> get_opt_range(const std::string & str)
{
    std::array<int,2> range;
    std::stringstream pss;
    std::vector<int> packets;
    int i = 0 ;
    std::string range_str;

    pss << str;
    while (std::getline(pss, range_str, '-'))
    {
        range[i] = std::stoi(range_str);
        ++i;
    }
    if (i != 0)
    {
        for (int j = range[0]; j <= range[1]; ++j)
        {
            packets.push_back(j);
        }
    }
    else
    {
        packets.push_back(std::stoi(str));
    }
    return packets;
}

void usage()
{
    std::cout << "tsparse [options] file" << std::endl;
    std::cout << "  Options:" << std::endl;
    std::cout << "          -f <file>           File that should be parsed" << std::endl;
    std::cout << "          -h                  help" << std::endl;
    std::cout << "          -t                  Print packets with pts" << std::endl;
    std::cout << "          -e                  Print packets with EBP markers" << std::endl;
    std::cout << "          -r                  Print packets with random access indicators" << std::endl;
    std::cout << "          -p <pid1,pid2...>   Print packets with pids, comma separated list of pids" << std::endl <<
    "                                           If no pids given, print a list of pids in transport stream" << std::endl;
    std::cout << "          -i <pkt1, pk2...>   Inspect (print) packet with given number, comma separated list of packets" << std::endl;
    std::cout << "          -s                  Print only payload start packets" << std::endl;
    std::cout << "          -x                  Print adaption field and PES header" << std::endl;
    std::cout << "          -X                  Print packet payload in hex" << std::endl;
}
