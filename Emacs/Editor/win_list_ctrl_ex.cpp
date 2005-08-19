#include <afxcmn.h>
#include <win_list_ctrl_ex.h>


/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
	: CListCtrl()
	, next_unique_lparam(1)
	{}

CListCtrlEx::~CListCtrlEx()
	{}

BOOL CListCtrlEx::AddColumn(LPCTSTR strItem,int nItem,int nSubItem,int nMask,int nFmt)
	{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = GetStringWidth(lvc.pszText) + 15;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return InsertColumn(nItem,&lvc);
	}

BOOL CListCtrlEx::AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
	{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	if( strItem[0] == '\0' )
		lvItem.pszText = " ";
	else
		lvItem.pszText = (LPTSTR) strItem;
	if(nImageIndex != -1){
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage |= LVIF_IMAGE;
	}
	if(nSubItem == 0)
		{
		lvItem.mask |= LVIF_PARAM;
		lvItem.lParam = next_unique_lparam++;
		return InsertItem(&lvItem);
		}
	return SetItem(&lvItem);
	}

/////////////////////////////////////////////////////////////////////////////

