
// SampleAppDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CSampleAppDlg ダイアログ
class CSampleAppDlg : public CDialog
{
// コンストラクション
public:
	CSampleAppDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_SAMPLEAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CStatic m_STA_1;
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    CButton m_CHK_1;
    CButton m_RAD_1;
    CButton m_RAD_2;
    CButton m_RAD_3;
    CStatic m_PIC_1;
    CComboBox m_CMB_1;
    CEdit m_EDIT_1;
};
