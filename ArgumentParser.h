//
// Created by Björn Hjortsberg on 2022-10-17.
//

#pragma once

#include "PacketFilter.h"
#include "OutputOptions.h"

class ArgumentParser {
public:
    ArgumentParser() = default;
    static std::tuple<PacketFilterPtr, OutputOptionsPtr, std::string> parse(int argc, char** argv);
};