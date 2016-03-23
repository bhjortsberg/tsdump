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
    void listPids();
    bool printPayload() const;
    bool printExtraInfo() const;
    bool listPidsOnly() const;

private:
    bool m_payload;
    bool m_extra;
    bool m_listPids;
};

typedef std::shared_ptr<OutputOptions> OutputOptionsPtr;

#endif //TSPARSE_OUTPUTOPTIONS_H
