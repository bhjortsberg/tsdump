//
// Created by Björn Hjortsberg on 2017-12-20.
//

#pragma once

#include "TSSource.h"
#include <condition_variable>

class SourceFactory
{
public:
    static TSSourcePtr create(const std::string& source,
                              std::condition_variable& cond,
                              std::mutex& mutex);
};