#include <iostream>
#include <sstream>
#include <unistd.h>
#include <array>
#include <getopt.h>
#include "SourceFactory.h"
#include "TransportStream.h"
#include "TSReport.h"
#include "PacketFilter.h"
#include "OutputOptions.h"

using namespace std;

void usage();
std::vector<int> get_opt_values(char * opt);
std::vector<std::string> get_opt_comma_separated(const char * opt);
std::vector<int> get_opt_range(const std::string & str);
std::vector<int> get_range(int from, int to);
std::vector<int> get_single_value(const std::string & str);

int main(int argc, char ** argv)
{
    PacketFilterPtr filter(new PacketFilter());
    OutputOptionsPtr option(new OutputOptions());
    int ch;

    if (argc < 2)
    {
        std::cerr << "Too few arguments" << std::endl;
        usage();
        return 0;
    }

    const int show_pids_only = 1001;
    struct option opts[1];
    opts[0].name = "pids";
    opts[0].has_arg = 0;
    opts[0].flag = nullptr;
    opts[0].val = show_pids_only;
    int longindex;

    while ((ch = getopt_long(argc, argv, "i:hp:tesrXxw:", opts, &longindex)) != -1) {
        std::string pids_str;
        std::vector<int> pids;
        std::stringstream ss;

        try
        {
            std::string filename;
            switch (ch)
            {
                case 'i':
                    filter->packets(get_opt_values(optarg));
                    break;
                case 'h':
                    usage();
                    return 0;
                case 'p':
                    filter->pids(get_opt_values(optarg));
                    break;
                case show_pids_only:
                    option->listPids();
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
                case 'w':
                    if (optarg)
                    {
                        filename = std::string(optarg);
                        option->outputFile(filename);
                    }
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
        catch (const FilterError& e)
        {
            std::cout << e.what();
            usage();
            return 0;
        }

    }

    std:string file_name;
    if (argv[optind] != 0)
    {
        file_name = std::string(argv[optind]);
    }

    try {

        std::mutex mutex;
        std::condition_variable cond;

        TSSourcePtr source = SourceFactory::create(file_name, cond, mutex);
        TransportStream ts(std::move(source), cond, mutex);
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
        try {

            auto p = get_opt_range(a);

            packets.insert(std::end(packets), std::begin(p), std::end(p));
        }
        catch (std::invalid_argument & e) {
            break;
        }
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
    if (not opt)
    {
        return vec;
    }
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
        packet_numbers = get_range(range[0], range[1]);
    }
    else
    {
        packet_numbers = get_single_value(str);
    }
    return packet_numbers;
}

std::vector<int> get_range(int from, int to)
{
    std::vector<int> range;
    for (int j = from; j <= to; ++j)
    {
        range.push_back(j);
    }

    return range;
}

std::vector<int> get_single_value(const std::string & str)
{
    std::vector<int> packet;
    try {
        packet.push_back(std::stoi(str));
    } catch (std::invalid_argument & e)
    {
        // This is ok. return empty vector
    }
    return packet;
}

void usage()
{
    std::cout << "tsparse <source> [options]" << std::endl;
    std::cout << "\n";
    std::cout << "  <source>                    Source where transport stream is read.\n"
                 "                              Can be a file or multicast address on the\n"
                 "                              form <ip>:<port>\n";
    std::cout << "  Options:\n";
    std::cout << "          -h                  help\n";
    std::cout << "          -t                  Print packets with pts\n";
    std::cout << "          -e                  Print packets with EBP markers\n";
    std::cout << "          -r                  Print packets with random access indicators\n";
    std::cout << "          -p <pid1,pid2...>   Print only packets with specified pids, comma separated\n"
                 "                              list of pids\n";
    std::cout << "          --pids              Print a list of pids in the transport stream\n";
    std::cout << "          -i <pkt1, pk2...>   Inspect (print) packet with given number, range or comma\n"
                 "                              separated list of packets\n";
    std::cout << "          -s                  Print only payload start packets\n";
    std::cout << "          -w <file>           Write data for printed packets to <file>\n";
    std::cout << "          -x                  Print adaptation field and PES header\n";
    std::cout << "          -X                  Print packet payload in hex\n";
    std::cout << "\n";
}
