
// SampleAppDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SampleApp.h"
#include "SampleAppDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <vector>

#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include "TestLibrary/TestLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSampleAppDlg ダイアログ

bool		CSampleAppDlg::m_bTestFlag;	



CSampleAppDlg::CSampleAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSampleAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STA_1, m_STA_1);
    DDX_Control(pDX, IDC_CHECK1, m_CHK_1);
    DDX_Control(pDX, IDC_RADIO1, m_RAD_1);
    DDX_Control(pDX, IDC_RADIO2, m_RAD_2);
    DDX_Control(pDX, IDC_RADIO3, m_RAD_3);
    DDX_Control(pDX, IDC_STA_2, m_PIC_1);
    DDX_Control(pDX, IDC_COMBO1, m_CMB_1);
    DDX_Control(pDX, IDC_EDIT1, m_EDIT_1);
}

BEGIN_MESSAGE_MAP(CSampleAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSampleAppDlg メッセージ ハンドラー

BOOL CSampleAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
    m_CMB_1.AddString(_T("猫"));
    m_CMB_1.AddString(_T("犬"));

    return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CSampleAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/// 表示されているダイアログを得る
CSampleAppDlg *GetMainDlg() {
    return static_cast<CSampleAppDlg*>( AfxGetApp()->m_pMainWnd );
}

///////////////////////////////////////////////////////////////////////////////////
void CPY_CaptureScreen(const std::wstring &szFilePath) {

    AutoGuiTestSupport::CaptureScreen( GetMainDlg(), szFilePath );
}

///////////////////////////////////////////////////////////////////////////////////
// ダイアログを出したり消したり
void CPY_ShowWindow(bool bShow) {

    auto pDlg = GetMainDlg();
    pDlg->ShowWindow(bShow ? SW_SHOW : SW_HIDE );
}

///////////////////////////////////////////////////////////////////////////////////
// メッセージを表示
void CPY_ShowMessage( const std::wstring &mes) {

    auto pDlg = GetMainDlg();
    pDlg->m_STA_1.SetWindowText( CString(mes.c_str()) );
}

///////////////////////////////////////////////////////////////////////////////////
/// チェックボックスを制御
void CPY_SetChekcBox( bool bSet ) {

    auto pDlg = GetMainDlg();
    pDlg->m_CHK_1.SetCheck( bSet ? BST_CHECKED : BST_UNCHECKED );
}

///////////////////////////////////////////////////////////////////////////////////
/// ラジオボタンを制御
void CPY_SetRadio( int nSel ) {

    auto pDlg = GetMainDlg();
    pDlg->m_RAD_1.SetCheck( nSel == 0 ? BST_CHECKED : BST_UNCHECKED );
    pDlg->m_RAD_2.SetCheck( nSel == 1 ? BST_CHECKED : BST_UNCHECKED );
    pDlg->m_RAD_3.SetCheck( nSel == 2 ? BST_CHECKED : BST_UNCHECKED );
}

///////////////////////////////////////////////////////////////////////////////////
/// エディットボックスの制御
void CPY_SetEditBox( const std::wstring &mes ) {

    auto pDlg = GetMainDlg();
    pDlg->m_EDIT_1.SetWindowText( mes.c_str() );
}

///////////////////////////////////////////////////////////////////////////////////
/// コンボボックスの制御
void CPY_SetComboBox( int nInd ) {

    auto pDlg = GetMainDlg();
    pDlg->m_CMB_1.SetCurSel( nInd );
}

///////////////////////////////////////////////////////////////////////////////////
/// 表示設定
void CPY_SetPicture( int nInd ) {

    auto pDlg = GetMainDlg();
    CRect rectDraw;
    pDlg->m_PIC_1.GetWindowRect( &rectDraw );
    pDlg->ScreenToClient( &rectDraw );

    CClientDC dc( pDlg );

    dc.FillRect( &rectDraw, &CBrush(RGB(0,0,0)));

    CPen pen1( PS_SOLID, 1, RGB(0xff,0xff,0xff));
    auto pOldPen = dc.SelectObject( &pen1 );

    if ( nInd == 0 ) {

        dc.MoveTo( rectDraw.left, rectDraw.top );
        dc.LineTo(rectDraw.right, rectDraw.bottom);
        dc.MoveTo( rectDraw.right, rectDraw.top );
        dc.LineTo(rectDraw.left, rectDraw.bottom);
    }
    else {

        dc.Ellipse(rectDraw.left, rectDraw.top, rectDraw.right, rectDraw.bottom );
    }

    dc.SelectObject( pOldPen );
}

///////////////////////////////////////////////////////////////////////////////////
// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CSampleAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        /////////////////////////////////////////////////////////////////////
        /// Pythonスクリプトを読み込んで実行する部分
        /////////////////////////////////////////////////////////////////////
        static bool bFirstFlag = true;  // 一回だけ実行するために
        if ( bFirstFlag ) {

            bFirstFlag = false;
			
			if ( m_bTestFlag ) {
				RunTest();
			}
        }


        CDialog::OnPaint();
    }
}

// テストを実行する
void CSampleAppDlg::RunTest() {

	// Python インタプリタ初期化
	Py_Initialize();

	// グローバルの名前空間を取得
	auto main_namespace = boost::python::import("__main__").attr("__dict__");

	// 関数をPythonに登録する
	main_namespace["UT_ShowWindow"] = &CPY_ShowWindow;
	main_namespace["UT_Sleep"] = &AutoGuiTestSupport::Sleep;
	main_namespace["UT_ShowMessage"] = &CPY_ShowMessage;
	main_namespace["UT_CaptureScreen"] = &CPY_CaptureScreen;
	main_namespace["UT_Terminate"]  = &AutoGuiTestSupport::ExitProgramSDI;

	main_namespace["UT_SetChekcBox"]  = &CPY_SetChekcBox;
	main_namespace["UT_SetRadio"]  = &CPY_SetRadio;
	main_namespace["UT_SetEditBox"]  = &CPY_SetEditBox;
	main_namespace["UT_SetComboBox"]  = &CPY_SetComboBox;
	main_namespace["UT_SetPicture"]  = &CPY_SetPicture;

	// Python のコードを実行
	try {
		boost::python::exec_file( 
			"test_script.py", main_namespace, main_namespace 
			);
	}
	catch(boost::python::error_already_set const &)
	{
		// エラーを表示する
		// http://stackoverflow.com/questions/1418015/how-to-get-python-exception-text
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);

		boost::python::handle<> hType(ptype);
		boost::python::object extype(hType);
		boost::python::handle<> hTraceback(ptraceback);
		boost::python::object traceback(hTraceback);

		// Extract error message
		std::string strErrorMessage = boost::python::extract<std::string>(pvalue);

		// Extract line number (top entry of call stack)
		// if you want to extract another levels of call stack
		// also process traceback.attr("tb_next") recurently
		long lineno = boost::python::extract<long> (traceback.attr("tb_lineno"));
		std::string filename = boost::python::extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
		std::string funcname = boost::python::extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));

		CStringA strError;
		strError.Format( "%s %s %d %s", filename.c_str(), funcname.c_str(), lineno, strErrorMessage.c_str() );
		AfxMessageBox( CString(strError) );
	}   
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSampleAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CSampleAppDlg::OnEraseBkgnd(CDC* pDC)
{
    auto nRet = CDialog::OnEraseBkgnd(pDC);
    return nRet;
}


HBRUSH CSampleAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;
    // 背景色を乱数で変える
//    static CBrush brash;
//    brash.CreateSolidBrush(rand() & 0xffffff );

//    return static_cast<HBRUSH>( brash.Detach() );
}
