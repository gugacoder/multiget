#ifndef _RIGHT_TREE_H
#define _RIGHT_TREE_H

#include <wx/wx.h>
#include <wx/treectrl.h> 
//#include <iostream> //temp
#include "common.h"

class CRightDownSplitter;

class CRightTree : public wxTreeCtrl
{

public:
    CRightTree( wxWindow* parent );
    void ShowTask( _TaskAttr* task );
    void DynamicLang();

protected:
    _TASK_SHOWTYPE m_nMode; //当前所在模式
    void OnSelectChanged( wxTreeEvent& event );
    void OnErase( wxEraseEvent& event );
    int m_nLastSelectID;  //最后选择的ID，即条目数据

private:
    DECLARE_EVENT_TABLE()
};

#endif

