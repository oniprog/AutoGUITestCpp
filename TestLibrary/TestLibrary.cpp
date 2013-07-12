////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  テスト用ライブラリ
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include "TestLibrary/TestLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace AutoGuiTestSupport {

////////////////////////////////////////////////////////////////////////////
/// GUIプログラムに対してコンソール画面を表示する
void ShowConsole() {

    AllocConsole();

    //標準出力関連付け
    int hStdOut = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    *stdout = *::_tfdopen(hStdOut, TEXT("w"));
    ::setvbuf(stdout, NULL, _IONBF, 0);

    //標準エラー出力関連付け
    int hStdErr = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    *stderr = *::_tfdopen(hStdErr, TEXT("w"));
    ::setvbuf(stderr, NULL, _IONBF, 0);
}

////////////////////////////////////////////////////////////////////////////
/// GUIプログラムを終了する
void ExitProgramSDI() {
    
    // 保存画面が出ないように．
    CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
    if ( pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd) ) && pFrame && pFrame->GetActiveDocument() )
        pFrame->GetActiveDocument()->SetModifiedFlag(FALSE);

    // プログラムの終了コマンドを送付
    AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

///////////////////////////////////////////////////////////////////////////////////
/// 指定窓の画面をキャプチャして指定ファイルにPNG形式で保存する
void CaptureScreen(CWnd *pWnd, const std::wstring &szFilePath) {

    CRect rectClient;
    pWnd->GetClientRect( &rectClient );

    CClientDC dc(pWnd);

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
/// スリープ時間を秒単位で指定する．0.5秒等も指定可能
void Sleep(double dTime) {

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

}; //namespace AutoGuiTestSupport

