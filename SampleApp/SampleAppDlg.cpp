
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSampleAppDlg ダイアログ



CSampleAppDlg::CSampleAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STA_1, m_STA_1);
}

BEGIN_MESSAGE_MAP(CSampleAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSampleAppDlg メッセージ ハンドラー

BOOL CSampleAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
		CDialogEx::OnSysCommand(nID, lParam);
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
/// 画面をキャプチャする
void CPY_CaptureScreen(const std::wstring &szFilePath) {

    auto pDlg = GetMainDlg();

    CRect rectClient;
    pDlg->GetClientRect( &rectClient );

    CClientDC dc(pDlg);

    CDC dc2;
    dc2.CreateCompatibleDC( &dc );

    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height() );
    
    auto pOldBitmap = dc2.SelectObject( &bitmap );

    dc2.BitBlt(0,0, rectClient.Width(), rectClient.Height(), &dc, 0, 0, SRCCOPY );

    int nWidthAdjust = rectClient.Width();
    if ( nWidthAdjust % 4 ) { nWidthAdjust += 4 - nWidthAdjust % 4;}

    std::vector<BYTE> bitdata( nWidthAdjust * rectClient.Height() * 3 );
    
    dc2.SelectObject( pOldBitmap );

    BITMAPINFO bitmapinfo;
    ZeroMemory(&bitmapinfo, sizeof(BITMAPINFO) );
    bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapinfo.bmiHeader.biWidth = nWidthAdjust;
    bitmapinfo.bmiHeader.biHeight = rectClient.Height();
    bitmapinfo.bmiHeader.biPlanes = 1;
    bitmapinfo.bmiHeader.biBitCount = 24;
    bitmapinfo.bmiHeader.biCompression = BI_RGB;
    GetDIBits( dc2, bitmap, 0, rectClient.Height(), bitdata.data(), &bitmapinfo, DIB_RGB_COLORS );


    boost::gil::rgb8_image_t img( nWidthAdjust, rectClient.Height() );
    copy_pixels(boost::gil::interleaved_view(nWidthAdjust, rectClient.Height(), (const boost::gil::rgb8_pixel_t*)bitdata.data(), nWidthAdjust*3), view(img));
    png_write_view(CStringA(szFilePath.c_str()), boost::gil::flipped_up_down_view( const_view(img) ) );
}

///////////////////////////////////////////////////////////////////////////////////
/// プログラムを終了する
void CPY_Terminate() {

    GetMainDlg()->EndDialog(0);
}

///////////////////////////////////////////////////////////////////////////////////
/// Sleep関数　引数は1が1秒待ち
void CPY_Sleep(double dTime) {

    DWORD nWaitTime = static_cast<int>( dTime * 1000.0 );

    DWORD nBeginTime = GetTickCount();

    MSG msg;
    while( GetTickCount() - nBeginTime < nWaitTime ) {
     
        Sleep(1);
        while(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
        {
            if (!AfxGetApp()->PumpMessage())
                break;
        }
    }
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

            // Python インタプリタ初期化
            Py_Initialize();

            // グローバルの名前空間を取得
            auto main_namespace = boost::python::import("__main__").attr("__dict__");

            // 関数をPythonに登録する
            main_namespace["UT_ShowWindow"] = &CPY_ShowWindow;
            main_namespace["UT_Sleep"] = &CPY_Sleep;
            main_namespace["UT_ShowMessage"] = &CPY_ShowMessage;
            main_namespace["UT_CaptureScreen"] = &CPY_CaptureScreen;
            main_namespace["UT_Terminate"]  = &CPY_Terminate;

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


        CDialogEx::OnPaint();
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
    auto nRet = CDialogEx::OnEraseBkgnd(pDC);
    return nRet;
}


HBRUSH CSampleAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    // 背景色を乱数で変える
    static CBrush brash;
    brash.CreateSolidBrush(rand() & 0xffffff );

    return static_cast<HBRUSH>( brash.Detach() );
}
