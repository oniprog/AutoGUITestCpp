////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  自動GUIテスト
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoGUITestCpp.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#define RESULT_FOLDER "Results"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

////////////////////////////////////////////////////////////////////////////
CString GetExeFolder() {

    wchar_t szPath[MAX_PATH];
    GetModuleFileName(nullptr, szPath, sizeof(szPath) );

    CString strPath = szPath;
    return strPath.Left( strPath.ReverseFind(_T('\\')));
}

////////////////////////////////////////////////////////////////////////////
/// ファイルを全部削除
void DeleteAllFile( CString strFolder ) {

    CFileFind findfile;
    BOOL bLoop = findfile.FindFile(strFolder + _T("\\*.*"));
    while(bLoop) {
        
        bLoop = findfile.FindNextFile();

        DeleteFile(findfile.GetFilePath());
    }
}

////////////////////////////////////////////////////////////////////////////
class AutoGUITest {
public:
    // テスト実行
    void    Run();

private:

    bool        LoadConfig();

    CString     m_strApplicationPath;
    CString     m_strTestFolder;
    DWORD       m_nKillTime;
};

////////////////////////////////////////////////////////////////////////////
/// テスト実行
void AutoGUITest::Run() {

    if ( !LoadConfig() )
        return;

    SetCurrentDirectory( GetExeFolder() );
    SetCurrentDirectory( m_strTestFolder );

    CFileFind filefind;
    BOOL bLoop = filefind.FindFile( _T("*.*"));
    while(bLoop) {

        bLoop = filefind.FindNextFile();

        // ディレクトリのみ処理する
        if( !filefind.IsDirectory() )
            continue;

        // "."と".."フォルダは無視する
        CString strFileName = filefind.GetFileName();
        if ( filefind.GetFileName() == _T(".") || filefind.GetFileName() == _T(".."))
            continue;

        // フォルダを移動する
        CString strTestFolder = filefind.GetFilePath();
        SetCurrentDirectory( strTestFolder );

        // 結果フォルダを作る(すでにあるときもある)，内容を消す
        CreateDirectory( _T(RESULT_FOLDER), nullptr );
        DeleteAllFile( _T(RESULT_FOLDER) );

        // Pythonスクリプトをコピーする
        CopyFile( _T("test_script.py"), _T(RESULT_FOLDER) _T("\\test_script.py"), FALSE/*上書き*/ );

        // カレントフォルダを変える
        SetCurrentDirectory( _T(RESULT_FOLDER) );

        // アプリを実行する
        STARTUPINFO si = { sizeof(STARTUPINFO) };
        PROCESS_INFORMATION pi = {};
        if (!CreateProcess( NULL, m_strApplicationPath.GetBuffer(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi ) ) {  

            AfxMessageBox(_T("アプリが実行できませんでした"));
            continue;
        }


        CloseHandle(pi.hThread);
        if ( WaitForSingleObject(pi.hProcess, m_nKillTime ) == WAIT_TIMEOUT ) {
            TerminateProcess( pi.hProcess, 0 );
        }

        // 結果を比較する
#if 0
        boost::gil::rgb8_image_t img( nWidthAdjust, rectClient.Height() );
        copy_pixels(boost::gil::interleaved_view(nWidthAdjust, rectClient.Height(), (const boost::gil::rgb8_pixel_t*)bitdata.data(), nWidthAdjust*3), view(img));
        png_write_view(CStringA(szFilePath.c_str()), boost::gil::flipped_up_down_view( const_view(img) ) );
#endif
    }
}

////////////////////////////////////////////////////////////////////////////
/// 設定を読み込む
bool AutoGUITest::LoadConfig() {

    CStringA strConfigPath( GetExeFolder() + _T("\\config.txt") );

    boost::property_tree::ptree p;
    read_json( std::string(strConfigPath), p );

    m_strApplicationPath = CString(p.get<std::string>("ApplicationPath").c_str());
    m_strTestFolder = CString(p.get<std::string>("TestFolder").c_str());
    m_nKillTime = p.get<DWORD>("KillTime");

    return true;    
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// MFC を初期化して、エラーの場合は結果を印刷します。
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 必要に応じてエラー コードを変更してください。
			_tprintf(_T("致命的なエラー: MFC の初期化ができませんでした。\n"));
			nRetCode = 1;
		}
		else
		{
            AutoGUITest test;
            test.Run();
		}
	}
	else
	{
		// TODO: 必要に応じてエラー コードを変更してください。
		_tprintf(_T("致命的なエラー: GetModuleHandle が失敗しました\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
