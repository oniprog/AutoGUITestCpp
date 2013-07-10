////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  自動GUIテスト
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoGUITestCpp.h"

#include <tuple>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/asio.hpp>

#include "AutoGUIHtmlWriter.h"

#define RESULT_FOLDER "Result"
#define CORRECT_FOLDER "Correct"
#define DIFF_FOLDER "Diff"

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
/// パスのフォルダ部分を取り出す
CString GetFolder( CString strPath ) {

    int nIndex = strPath.ReverseFind(_T('\\'));
    if ( nIndex >= 0 )
        return strPath.Left(nIndex);
    else
        return strPath;
}
////////////////////////////////////////////////////////////////////////////
/// パスのファイル部分を取り出す
CString GetFileName( CString strPath ) {

    int nIndex = strPath.ReverseFind(_T('\\'));
    if ( nIndex >= 0 )
        return strPath.Mid(nIndex+1);
    else
        return strPath;
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

    void        RunInside();

    HTMLWriter  m_writer;
    CString     m_strApplicationPath;  // 実行するアプリのパス
    CString     m_strTestFolder;        // 実行するテストがあるパス
    CString     m_strOutputHTML;        // 出力するHTMLのパス
    DWORD       m_nKillTime;            // 実行したアプリをKILLするまでの時間
    int         m_nServerPort;          // サーバーポート．0以外のとき，そのポートでひたすら待つ
    int         m_nErrorValue;          // この値以上のエラーが出たら報告する
};

////////////////////////////////////////////////////////////////////////////
// xorを画像に適用するためのファンクタ
class transform_xor {
public:
    template<class PixelT>
    PixelT  operator()( PixelT p1, PixelT p2 ) {
        PixelT ret;
        ret[0] = p1[0] ^ p2[0];
        ret[1] = p1[1] ^ p2[1];
        ret[2] = p1[2] ^ p2[2];
        return ret;
    }
};

////////////////////////////////////////////////////////////////////////////
/// テスト実行
void AutoGUITest::Run() {

    if ( !LoadConfig() )
        return;

    if ( m_nServerPort <= 0 ) { 
        // 一回だけ実行
        RunInside();
    }
    else {
        std::cout << "start as server : port[" << m_nServerPort << "] " << std::endl;

        // サーバー化　複数回実行の例
        boost::asio::io_service io_service;
        try {
            for (;;)
            {
                boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_nServerPort));
                boost::asio::ip::tcp::socket socket(io_service);
                acceptor.accept(socket);
                acceptor.close();

                std::cout << "run one test" << std::endl;

                RunInside();

                std::cout << "finished." << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }        
    }
}
////////////////////////////////////////////////////////////////////////////
/// テスト実行
void AutoGUITest::RunInside() {

    SetCurrentDirectory( GetExeFolder() );
    SetCurrentDirectory( m_strTestFolder );

    // HTML出力用フォルダの作成
    m_writer.Open( m_strOutputHTML );  // 作れなくても無視する
    m_writer.OutputHeader();

    // 
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

        // フォルダ位置を待避する
        TCHAR szPushFolder[ MAX_PATH ];
        GetCurrentDirectory(MAX_PATH, szPushFolder);

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

        // テスト開始
        CString strTestName = GetFileName(strTestFolder);
        m_writer.BeginTest(strTestName);

        // 終了待ち
        CloseHandle(pi.hThread);
        if ( WaitForSingleObject(pi.hProcess, m_nKillTime ) == WAIT_TIMEOUT ) {
            // 正常終了してないのでKillする
            TerminateProcess( pi.hProcess, 0 );
            m_writer.OutputForceTerminateError();
        }

        // フォルダを復帰する
        SetCurrentDirectory(szPushFolder);

        // 結果を比較する
        CFileFind findfile2;
        BOOL bLoop2 = findfile2.FindFile( _T(RESULT_FOLDER) _T("\\*.png"));
        while(bLoop2 ) {

            bLoop2 = findfile2.FindNextFile();

            CStringA strPngPath( findfile2.GetFilePath() );
            CStringA strPngFileName = strPngPath.Mid( strPngPath.ReverseFind(_T('\\'))+1 );
            strPngPath = CStringA(RESULT_FOLDER) + "\\" + strPngFileName;
            CStringA strCorrectPath = CStringA(CORRECT_FOLDER) + "\\" + strPngFileName; 
            CStringA strDiffPath = CStringA(DIFF_FOLDER) + "\\" + strPngFileName;

            boost::gil::rgb8_image_t img;
            png_read_image( strPngPath, img );

            // 比較用の正解ファイルをチェックする
            if ( PathFileExistsA(strCorrectPath)) {

                CreateDirectory(_T(DIFF_FOLDER), nullptr );

                // 比較を行う
                boost::gil::rgb8_image_t img2;
                png_read_image( strCorrectPath, img2 );

                if ( img.dimensions() != img2.dimensions() ) {

                    // 画像サイズが異なる
                    m_writer.OutputTestFileHeader( CStringW(strPngFileName) );

                    m_writer.OutputImageSizeDiffrentError();
                    m_writer.AddCompareImage( strTestName + _T("\\") + CStringW(strCorrectPath), strTestName + _T("\\") + CStringW(strPngPath) );
                }
                else {

                    // 比較する
                    boost::gil::rgb8_image_t img_diff(img.dimensions());
                    transform_pixels( const_view(img), const_view(img2), view(img_diff), transform_xor() );

                    png_write_view(strDiffPath, const_view(img_diff) );

                    // 0以外の場所の数を数える
                    int nErrorCnt = std::count_if( const_view(img_diff).begin(), const_view(img_diff).end(), [](boost::gil::rgb8c_pixel_t pD) { return pD[0]!=0 || pD[1]!=0 || pD[2]!=0; } );

                    if ( nErrorCnt > m_nErrorValue ) {
                        m_writer.OutputTestFileHeader( CStringW(strPngFileName) );

                        m_writer.OutputErrorValue( nErrorCnt );
                        m_writer.AddCompareImage( strTestName + _T("\\") + CStringW(strCorrectPath), strTestName + _T("\\") + CStringW(strPngPath) );
                        m_writer.AddDiffImage( strTestName + _T("\\") + CStringW(strDiffPath) );
                    }
                }
            }
            else {

                CreateDirectory( _T(CORRECT_FOLDER), nullptr );

                // 新規ファイルなので正解フォルダにコピーする
                CopyFileA( strPngPath, strCorrectPath, FALSE );
            }
        }

        // 1つのテストの終了
        m_writer.EndTest();
    }

    m_writer.OutputFooter();
}
  
////////////////////////////////////////////////////////////////////////////
/// 設定を読み込む
bool AutoGUITest::LoadConfig() {

    CStringA strConfigPath( GetExeFolder() + _T("\\config.txt") );

    boost::property_tree::ptree p;
    read_json( std::string(strConfigPath), p );

    m_strApplicationPath = CString(p.get<std::string>("ApplicationPath").c_str());
    m_strTestFolder = CString(p.get<std::string>("TestFolder").c_str());
    m_strOutputHTML = CString(p.get<std::string>("OutputHTML").c_str() );
    m_nKillTime = p.get<DWORD>("KillTime");
    m_nServerPort = p.get<int>("ServerPort");
    m_nErrorValue = p.get<int>("ErrorValue");

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
