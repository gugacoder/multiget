/*
��边的树控件
��个控件有四种模式
0、什么也没选择，控件不需要显示任何东西
1、在左边的树控件选择了正在下载的任务时，列表有 图表，线程1，线程2...
2、在左边的树控件选择了暂停完成的任务是，列表显示文件的基本信息，有详细信息，链接等。
3、在左边的树控件选择了已完成的任务时，列表显示文件的基本信息，有详细信息，链接等。

��数SwitchMode()用于切换这三种模式

��个控件的parent是第二个垂直分割控件，当这边有特定的选择时，右边需要做相应的切换。
��过调用parent提供的接口来通知parent切换显示界面。

��行中的任务这么来设置控制树

��息-\	(0)
|
|
信息提取 (-1)
|
|
文件管理 (-2)
|
|
图表进度 (-3)
|
|
线程1  (1)
|
|
....
*/
#include "righttree.h"
#include "rightdownsplitter.h"
#include "righttreeitemdata.h"
#include "mgapp.h"
#include "./icons/info.xpm"
#include "./icons/ant.xpm"
#include "./icons/diag.xpm"
#include "./icons/driver.xpm"
#include "./newicons/16/search.xpm"
#include "./newicons/16/detail.xpm"
#include "./icons/log.xpm"
#include <wx/imaglist.h>
#include <iostream>

using namespace std;

BEGIN_EVENT_TABLE( CRightTree, wxTreeCtrl )
EVT_TREE_SEL_CHANGED( -1, CRightTree::OnSelectChanged )
EVT_ERASE_BACKGROUND( CRightTree::OnErase )
END_EVENT_TABLE()

CRightTree::CRightTree( wxWindow* parent )
        : wxTreeCtrl( parent, -1, wxDefaultPosition, wxDefaultSize , wxTR_DEFAULT_STYLE | wxFULL_REPAINT_ON_RESIZE )
{
    wxImageList * imlist = new wxImageList( 16, 16, true, 7 );
    imlist->Add( wxIcon( info_xpm ) );
    imlist->Add( wxIcon( diag_xpm ) );
    imlist->Add( wxIcon( ant_xpm ) );
    imlist->Add( wxIcon( driver_xpm ) );
    imlist->Add( wxIcon( search_xpm ) );
    imlist->Add( wxIcon( log_xpm ) );
    imlist->Add( wxIcon( detail_xpm ) );
    AssignImageList( imlist );

}


void CRightTree::OnSelectChanged( wxTreeEvent& event )
{
	
    wxTreeItemId id = event.GetItem();
    CRightTreeItemData *pdata = ( CRightTreeItemData * ) GetItemData( id );
	if( pdata == NULL ) return;
    m_nLastSelectID = pdata->GetData(); 
    ( ( CRightDownSplitter* ) GetParent() ) ->OnRightTreeSelectItem( pdata->GetData() );

    //event.Skip();
}


void CRightTree::DynamicLang()
{

    wxTreeItemId child;
    wxTreeItemIdValue cookie;
    wxTreeItemId rid = GetRootItem(); 

    if ( !rid.IsOk() )
        return ;

    SetItemText( rid, _("Information") );

    child = GetFirstChild( rid, cookie );

    do
    {

        if ( !child.IsOk() )
            break;

        CRightTreeItemData* pdata = ( CRightTreeItemData* ) GetItemData( child );

        switch ( pdata->GetData() )
        {

            case - 1:
            SetItemText( child, _("Pioneer") );
            break;

            case - 2:
            SetItemText( child, _("File Info") );
            break;

            case - 3:
            SetItemText( child, _("Progress") );
            break;

            case 1:

            case 2:

            case 3:

            case 4:

            case 5:

            case 6:

            case 7:

            case 8:

            case 9:

            case 10:
            {
                wxString tx;
                tx.Printf( _("Thread%d"), pdata->GetData() );
                SetItemText( child, tx );
            }

            break;

            case 20:
            SetItemText( child, _("Detail") );
            break;

            case 21:
            SetItemText( child, _("Log") );
            break;

            default:
            break;
        }

        child = GetNextChild( rid, cookie );
    }
    while ( 1 );

}

void CRightTree::ShowTask( _TaskAttr* task )
{
    DeleteAllItems();

    if ( task == NULL )
    {
        wxTreeItemId rid;
        rid = AddRoot( _("Information"),
                       0,  //img
                       -1,  //sel img = img
                       new CRightTreeItemData( 0 ) );

        m_nLastSelectID = 0;
        SelectItem( rid );
        return ;
    }

    switch ( task->nStatus )
    {

        case _STATUS_RUNNING:
        {

            wxTreeItemId rid, old, tmp;
            bool oldfind = false;

            rid = AddRoot( _("Information"),
                           0,
                           -1,
                           new CRightTreeItemData( 0 ) );

            if ( m_nLastSelectID == 0 )
            {
                old = rid;
                oldfind = true;
            }

            tmp = AppendItem( rid,
                              _("Pioneer"),
                              4,
                              -1,
                              new CRightTreeItemData( -1 ) );

            if ( m_nLastSelectID == -1 )
            {
                old = tmp;
                oldfind = true;
            }

            tmp = AppendItem( rid,
                              _("File Info"),
                              3,
                              -1,
                              new CRightTreeItemData( -2 ) );

            if ( m_nLastSelectID == -2 )
            {
                old = tmp;
                oldfind = true;
            }

            tmp = AppendItem( rid,
                              _("Progress"),
                              1,
                              -1,
                              new CRightTreeItemData( -3 ) );

            if ( m_nLastSelectID == -3 )
            {
                old = tmp;
                oldfind = true;
            }


            for ( int i = 0; i < task->nThread; i++ )
            {
                wxString tx;
                tx.Printf( _("Thread%d"), i + 1 );
                tmp = AppendItem( rid, tx, 2, -1,
                                  new CRightTreeItemData( i + 1 ) );

                if ( m_nLastSelectID == i + 1 )
                {
                    old = tmp;
                    oldfind = true;
                }
            }


            Expand( rid );


            if ( oldfind )
            {
                SelectItem( old );
            }
            else
            {

                SelectItem( rid );
                ( ( CRightDownSplitter* ) GetParent() ) ->OnRightTreeSelectItem( 0 );
            }
        }

        break;

        case _STATUS_PAUSE:

        case _STATUS_WRONG:
        //case _STATUS_FINISH:

        case _STATUS_FINISH_SOFT:

        case _STATUS_FINISH_PACK:

        case _STATUS_FINISH_MUSIC:

        case _STATUS_FINISH_ISO:

        case _STATUS_FINISH_MOVIE:

        case _STATUS_FINISH_PUBLISH:

        case _STATUS_FINISH_PICTURE:

        case _STATUS_FINISH_UNKNOW:

        case _STATUS_DELETE:

        case _STATUS_WAITING:
        {
            wxTreeItemId rid, old, tmp;
            bool oldfind = false;
            rid = AddRoot( _("Information"),
                           0,
                           -1,
                           new CRightTreeItemData( 0 ) );

            if ( m_nLastSelectID == 0 )
            {
                old = rid;
                oldfind = true;
            }

            tmp = AppendItem( rid, _("Detail"),
                              6,
                              -1,
                              new CRightTreeItemData( 20 ) );

            if ( m_nLastSelectID == 20 )
            {
                old = tmp;
                oldfind = true;
            }

            tmp = AppendItem( rid, _("Log"),
                              5,
                              -1,
                              new CRightTreeItemData( 21 ) );

            if ( m_nLastSelectID == 21 )
            {
                old = tmp;
                oldfind = true;
            }

            Expand( rid );


            if ( oldfind )
            {
                SelectItem( old );
            }
            else
            {
                SelectItem( rid );
                ( ( CRightDownSplitter* ) GetParent() ) ->OnRightTreeSelectItem( 0 );
            }
        }

        break;

        default:
        break;
    }
}

void CRightTree::OnErase( wxEraseEvent& event )
{
		event.Skip();
}

