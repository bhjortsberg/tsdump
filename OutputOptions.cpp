//
// Created by Björn Hjortsberg on 04/03/16.
//

#include "OutputOptions.h"


OutputOptions::~OutputOptions()
{
    if (m_outputFile.is_open())
    {
        m_outputFile.close();
    }
}

void OutputOptions::payload()
{
    m_payload = true;
}

bool OutputOptions::printPayload() const
{
    return m_payload;
}

void OutputOptions::extraInfo()
{
    m_extra = true;
}

bool OutputOptions::printExtraInfo() const
{
    return m_extra;
}

void OutputOptions::listPids()
{
    m_listPids = true;
}

bool OutputOptions::listPidsOnly() const
{
    return m_listPids;
}

void OutputOptions::outputFile(const std::string &outputFile)
{
    if (not outputFile.empty())
    {
        m_outputFile.open(outputFile, std::fstream::out | std::fstream::binary);
    }
}

bool OutputOptions::fileOutput() const
{
    return m_outputFile.is_open();
}

std::ofstream& OutputOptions::outputFile()
{
    return m_outputFile;
}
