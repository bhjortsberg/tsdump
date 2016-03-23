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
        std::string pids_str;
        std::vector<int> pids;
        std::stringstream ss;

        switch (ch) {
            case 'i':
                filter->packets(get_opt_values(optarg));
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
                if (!pids.empty())
                {
                    filter->pids(pids);
                }
                else{
                    option->listPids();
                }
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

    // TODO: Use a lambda, capture packets by reference
//    for_each(std::begin(ivec), std::end(ivec), get_opt_range);

    for (auto a : ivec)
    {
        auto p = get_opt_range(a);

        packets.insert(std::end(packets), std::begin(p), std::end(p));

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
    std::vector<int> packet_numbers;
    int i = 0 ;
    std::string range_str;

    pss << str;
    while (pss.str().find('-') != -1 && std::getline(pss, range_str, '-'))
    {
        range[i] = std::stoi(range_str);
        ++i;
    }
    if (i != 0)
    {
        for (int j = range[0]; j <= range[1]; ++j)
        {
            packet_numbers.push_back(j);
        }
    }
    else
    {
        packet_numbers.push_back(std::stoi(str));
    }
    return packet_numbers;
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

    std::cout << std::endl;
}
