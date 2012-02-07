
#ifndef _TASKLIST_H
#define _TASKLIST_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "common.h"

DECLARE_EVENT_TYPE( mgID_PRUN, -1 )
DECLARE_EVENT_TYPE( mgID_PSTOP, -1 )
DECLARE_EVENT_TYPE( mgID_PDELETE, -1 )
DECLARE_EVENT_TYPE( mgID_PROPERTY, -1 )
DECLARE_EVENT_TYPE( mgID_PRELOAD, -1 )

class CTaskList : public wxListView
{

public:
    CTaskList( wxWindow* parent );
    void AddTaskList( _TaskAttr& task );
    void UpdateTaskList( _TaskAttr& task );
    int SelectTask( int taskid, bool on = true );
    void IsTaskCanMove( int taskid, bool& canup, bool& candown );
    void MoveDown( int taskid, int& downid );
    void MoveUp( int taskid, int& downid );
    void DynamicLang();

protected:
    void OnSelect( wxListEvent& event );
    void OnErase( wxEraseEvent& event );
    void OnRightClick( wxListEvent& event );

    void OnRun( wxCommandEvent& event );
    void OnStop( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnProperty( wxCommandEvent& event );
    void OnReload( wxCommandEvent& event );
    wxString TimeToString( int seconds );

private:

    DECLARE_EVENT_TABLE()
};

#endif

