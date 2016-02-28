//
// Created by Björn Hjortsberg on 22/02/16.
//

#ifndef TSPARSE_IFILTER_H
#define TSPARSE_IFILTER_H

#include <memory>
#include "TSPacket.h"


class IFilter
{
public:
    virtual std::string filter(const TSPacket & packet) const = 0;
    virtual std::string statistics() const = 0;

};

typedef std::shared_ptr<IFilter> IFilterPtr;

#endif //TSPARSE_IFILTER_H
