#ifndef _TASK_DETAIL_REPORT_H
#define _TASK_DETAIL_REPORT_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "common.h"

class CTaskDetailReport : public wxListView
{

public:
    CTaskDetailReport( wxWindow* parent );
    void ShowTaskDetail( _TaskAttr& tsk );	//也可以用于更新显示
    void DynamicLang();

protected:
    int AddInfo( wxString name, wxString value );
    void OnSize( wxEvent& event );
    void OnColumnResize( wxListEvent& event );
    void OnErase( wxEraseEvent& event );
    DECLARE_EVENT_TABLE()

};

#endif

