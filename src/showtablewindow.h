/*
��示下载图表的滚动窗口

2006/07/12

*/

#include <wx/wx.h>
#include <wx/bitmap.h>
#include "common.h"
#include <vector>

DECLARE_EVENT_TYPE( mgID_8K, -1 )
DECLARE_EVENT_TYPE( mgID_16K, -1 )
DECLARE_EVENT_TYPE( mgID_32K, -1 )
DECLARE_EVENT_TYPE( mgID_64K, -1 )
DECLARE_EVENT_TYPE( mgID_128K, -1 )
DECLARE_EVENT_TYPE( mgID_256K, -1 )
DECLARE_EVENT_TYPE( mgID_512K, -1 )
DECLARE_EVENT_TYPE( mgID_1M, -1 )
DECLARE_EVENT_TYPE( mgID_2M, -1 )
DECLARE_EVENT_TYPE( mgID_4M, -1 )

class CShowTableWindow : public wxScrolledWindow
{

public:
    CShowTableWindow( wxWindow* parent );
    void SetRange( llong to, llong from , int nsize, bindex data[] ); //设置文件范围
    void SetUnit( int kbyte );	//设置显示单元
    void SetData( int segs, bindex data[] );  //设置当前的完成数据

protected:
    virtual void OnDraw( wxDC& dc );
    void DrawBlock( wxDC& dc, wxPoint& base, int line , int col, int color );
    int GetBlockColor( int totalline, int totalcol, int line, int col );
    void On8K( wxCommandEvent& event );
    void On16K( wxCommandEvent& event );
    void On32K( wxCommandEvent& event );
    void On64K( wxCommandEvent& event );
    void On128K( wxCommandEvent& event );
    void On256K( wxCommandEvent& event );
    void On512K( wxCommandEvent& event );
    void On1M( wxCommandEvent& event );
    void On2M( wxCommandEvent& event );
    void On4M( wxCommandEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnSize( wxSizeEvent& event );

private:
    wxBitmap m_GrayBlock;
    wxBitmap m_YellowBlock[ 10 ];
    wxBitmap m_GreenBlock;
    //开始显示位置
    llong m_from;
    //结束显示位置
    llong m_to;
    //缺省的显示单元大小，单位K（1024字节）
    int m_unit;
    //数据块的数量
    int m_nBlock;
    //强制刷新
    bool m_bForceUpdata;
    //保存的用于刷新的数据
    std::vector<bindex> m_DataList;

    DECLARE_EVENT_TABLE()
};

