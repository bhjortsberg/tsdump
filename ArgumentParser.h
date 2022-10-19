//
// Created by Björn Hjortsberg on 2022-10-17.
//

#pragma once

class ArgumentParser {
public:
    ArgumentParser(int argc, char ** argv);
    void parse();
};