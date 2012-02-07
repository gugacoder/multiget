
#ifndef _RIGHTTREE_ITEMDATA_H
#define _RIGHTTREE_ITEMDATA_H

#include <wx/wx.h>
#include <wx/treectrl.h>

class CRightTreeItemData : public wxTreeItemData
{

public:
    CRightTreeItemData( int ndata );
    int GetData();
    int m_nData; //指示本节点的类型
    //在上层插入树的节点时，这个数据要准确设置
    //这个定义在righttree.cpp中要用到
    /*
     
     0: 根节点，右边切换为无显示
    -1: 信息提取节点
    -2: 文件管理节点
    -3: 图表节点，右边切换为图表
     1: 线程1，右边切换为线程输出信息
     2: 线程2
     ...
     
     20: 详细信息
     21: 日志信息
    */

};


#endif

