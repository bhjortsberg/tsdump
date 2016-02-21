#include <iostream>
#include "TransportStream.h"

using namespace std;

int main(int argc, char ** argv)
{

    TransportStream ts(argv[1]);

    int ebp_cnt = 0, rai_cnt = 0;
    int i = 0;
    for (TSPacket packet : ts.getPackets()) {
        std::cout << i << " " << packet.pid();
        if (packet.has_pes_header())
            std::cout << std::hex << " 0x" << packet.pes_header().get_pts() << std::dec;

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

    return 0;
}