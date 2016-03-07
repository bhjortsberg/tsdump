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
    void extraInfo();
    bool printPayload() const;
    bool printExtraInfo() const;
private:
    bool m_payload;
    bool m_extra;
};

typedef std::shared_ptr<OutputOptions> OutputOptionsPtr;

#endif //TSPARSE_OUTPUTOPTIONS_H
