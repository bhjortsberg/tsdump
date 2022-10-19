#include <iostream>
#include <sstream>
#include <array>
#include "SourceFactory.h"
#include "TransportStream.h"
#include "TSReport.h"
#include "ArgumentParser.h"

using namespace std;

namespace {
std::function< void(int) > terminateTransportStream;

void signalHandler(int signal)
{
    terminateTransportStream(signal);
}
}

int main(int argc, char ** argv)
{
    auto [filter, option, fileName] = ArgumentParser::parse(argc, argv);

    try {

        std::mutex mutex;
        std::condition_variable cond;

        TSSourcePtr source = SourceFactory::create(fileName, cond, mutex);
        TransportStream ts(std::move(source), cond, mutex);
        TSReport report(ts, std::move(filter), option);

        terminateTransportStream = [&ts](int)
        {
            ts.stop();
        };

        report.report();

    } catch (std::runtime_error & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}