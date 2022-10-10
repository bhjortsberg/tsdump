//
// Created by Björn Hjortsberg on 2016-03-04.
//

#pragma once

#include <future>
#include <fstream>

class OutputOptions
{
public:
    OutputOptions() = default;
    ~OutputOptions();
    void payload();
    void extraInfo();
    void listPids();
    bool printPayload() const;
    bool printExtraInfo() const;
    bool listPidsOnly() const;
    void outputFile(const std::string& outputFile);
    std::ofstream& outputFile();
    bool fileOutput() const;

private:
    bool mPayload;
    bool mExtra;
    bool mListPids;
    std::ofstream mOutputFile;
};

using OutputOptionsPtr = std::shared_ptr<OutputOptions>;

