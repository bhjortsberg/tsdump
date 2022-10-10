//
// Created by Björn Hjortsberg on 2016-03-04.
//

#include "OutputOptions.h"


OutputOptions::~OutputOptions()
{
    if (mOutputFile.is_open())
    {
        mOutputFile.close();
    }
}

void OutputOptions::payload()
{
    mPayload = true;
}

bool OutputOptions::printPayload() const
{
    return mPayload;
}

void OutputOptions::extraInfo()
{
    mExtra = true;
}

bool OutputOptions::printExtraInfo() const
{
    return mExtra;
}

void OutputOptions::listPids()
{
    mListPids = true;
}

bool OutputOptions::listPidsOnly() const
{
    return mListPids;
}

void OutputOptions::outputFile(const std::string &outputFile)
{
    if (not outputFile.empty())
    {
        mOutputFile.open(outputFile, std::fstream::out | std::fstream::binary);
    }
}

bool OutputOptions::fileOutput() const
{
    return mOutputFile.is_open();
}

std::ofstream& OutputOptions::outputFile()
{
    return mOutputFile;
}
