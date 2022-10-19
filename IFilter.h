//
// Created by Björn Hjortsberg on 2016-02-22.
//

#pragma once

#include <memory>
#include "TSPacket.h"

class FilterError : public std::runtime_error
{
public:
    explicit FilterError(const std::string& message):
            std::runtime_error(message)
    {
    }
};

class IFilter
{
public:
    virtual bool show(const TSPacketPtr &packet) const = 0;
    virtual ~IFilter() = default;
};

using IFilterPtr = std::unique_ptr<IFilter>;