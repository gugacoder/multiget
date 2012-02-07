#ifndef _FILE_MD5_H
#define _FILE_MD5_H

#include <wx/thread.h>
#include "common.h"

class CFileMd5 : public wxThread
{

public:
    CFileMd5( _TaskAttr& task );
    virtual void* Entry();

protected:
    _TaskAttr* m_task;
};

#endif

