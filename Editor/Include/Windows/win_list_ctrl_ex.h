/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

#ifndef _win_list_ctrl_ex__h
#define _win_list_ctrl_ex__h

class CListCtrlEx : public CListCtrl
{
    // Attributes
protected:
    int next_unique_lparam;

    // Operation
public:
    CListCtrlEx();
    virtual ~CListCtrlEx();
    CImageList* SetImageList(CImageList* pImageList, int nImageListType = TVSIL_NORMAL);
    BOOL AddColumn(
        LPCTSTR strItem,int nItem,int nSubItem = -1,
        int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
        int nFmt = LVCFMT_LEFT);
    BOOL AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);
};

#endif // _win_list_ctrl_ex__h
