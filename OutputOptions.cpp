//
// Created by Björn Hjortsberg on 04/03/16.
//

#include "OutputOptions.h"

void OutputOptions::payload()
{
    m_payload = true;
}

bool OutputOptions::printPayload() const
{
    return m_payload;
}
