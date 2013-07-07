////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  HTML出力用クラス
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdio>

class HTMLWriter {

public:
    HTMLWriter();
    ~HTMLWriter();

    bool    Open(const wchar_t * szOutputPath );
    void    OutputHeader();
    void    OutputFooter();

    void    BeginTest(const wchar_t *szTestName );
    void    EndTest();

    // テストファイル名
    void    OutputTestFileHeader( const wchar_t *szTestFileName );

    void    AddCompareImage( const wchar_t *szPath, const wchar_t *szPath2 );
    void    AddDiffImage( const wchar_t *szDiffPath );

    // エラー値を出力する
    void    OutputErrorValue( int nErrorValue );

    // 強制終了エラー
    void    OutputForceTerminateError();

    // 画像のサイズが異なるエラー
    void    OutputImageSizeDiffrentError();

private:
    FILE*   m_wfp;
};