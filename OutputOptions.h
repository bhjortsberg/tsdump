//
// Created by Björn Hjortsberg on 04/03/16.
//

#ifndef TSPARSE_OUTPUTOPTIONS_H
#define TSPARSE_OUTPUTOPTIONS_H


#include <future>

class OutputOptions
{
public:
    void payload();
    bool printPayload() const;
private:
    bool m_payload;
};

typedef std::shared_ptr<OutputOptions> OutputOptionsPtr;

#endif //TSPARSE_OUTPUTOPTIONS_H
