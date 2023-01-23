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
    bool m_payload;
    bool m_extra;
    bool m_listPids;
    std::ofstream m_outputFile;
};

using OutputOptionsPtr = std::shared_ptr<OutputOptions>;

