////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  HTML出力用クラス
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoGUIHtmlWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMAGE_SIZE 400

////////////////////////////////////////////////////////////////////////////
HTMLWriter::HTMLWriter() : m_wfp(nullptr) {
}
////////////////////////////////////////////////////////////////////////////
HTMLWriter::~HTMLWriter() {
    if ( m_wfp )
    fclose(m_wfp);
}
////////////////////////////////////////////////////////////////////////////
bool HTMLWriter::Open(const wchar_t * szOutputPath ) {

    if( m_wfp )
        fclose(m_wfp);
    m_wfp = _tfopen(szOutputPath, _T("w,ccs=utf-8"));
    return m_wfp != nullptr;
}

////////////////////////////////////////////////////////////////////////////
void HTMLWriter::OutputHeader() {
    
    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("<!DOCTYPE html>\n"));
    _ftprintf(m_wfp, _T("<html lang=\"ja\">\n"));
    _ftprintf(m_wfp, _T("<head>\n"));
    _ftprintf(m_wfp, _T("<meta charset=\"utf-8\">\n"));
    _ftprintf(m_wfp, _T("<title>%s</title>\n"), _T("AutoGUITest Result"));
    _ftprintf(m_wfp, _T("<body>\n"));
}

////////////////////////////////////////////////////////////////////////////
void HTMLWriter::OutputFooter() {

    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("</body>\n"));
    _ftprintf(m_wfp, _T("</html>\n"));
}

////////////////////////////////////////////////////////////////////////////
// テスト１つ開始
void HTMLWriter::BeginTest(const wchar_t *szTestName ) {
    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("<HR>\n"));
    _ftprintf(m_wfp, _T("<H2>%s</H2>\n"), szTestName );
}
////////////////////////////////////////////////////////////////////////////
// テスト１つ終了
void HTMLWriter::EndTest() {
    if ( !m_wfp )
        return;

}

////////////////////////////////////////////////////////////////////////////
// テストファイル名
void HTMLWriter::OutputTestFileHeader( const wchar_t *szTestFileName ) {

    if ( !m_wfp )
        return;
    _ftprintf(m_wfp, _T("<H3>%s</H3><BR>\n"), szTestFileName );
}

////////////////////////////////////////////////////////////////////////////
// エラー値を出力する
void HTMLWriter::OutputErrorValue( int nErrorValue ) {

    if ( !m_wfp )
        return;

    _ftprintf( m_wfp, _T("<P>Error Value %d</P>"), nErrorValue );
}

////////////////////////////////////////////////////////////////////////////
/// 比較用の2つのイメージを追加する
void HTMLWriter::AddCompareImage( const wchar_t *szPath, const wchar_t *szPath2 ) {

    if ( !m_wfp )
        return;
    _ftprintf(m_wfp, _T("<IMG SRC=\"%s\" WIDTH=%d>\n"), szPath, IMAGE_SIZE, IMAGE_SIZE );
    _ftprintf(m_wfp, _T("<IMG SRC=\"%s\" WIDTH=%d>\n"), szPath2, IMAGE_SIZE, IMAGE_SIZE );
}
////////////////////////////////////////////////////////////////////////////
/// DIFFイメージを追加する
void HTMLWriter::AddDiffImage( const wchar_t *szDiffPath )
{
    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("<BR>"));
    _ftprintf(m_wfp, _T("<IMG SRC=\"%s\" WIDTH=%d>\n"), szDiffPath, IMAGE_SIZE, IMAGE_SIZE );
    _ftprintf(m_wfp, _T("<BR>"));
}

////////////////////////////////////////////////////////////////////////////
// 強制終了エラー
void HTMLWriter::OutputForceTerminateError() {

    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("<H3>プロセスを強制終了しました</H3><BR>\n"));
}

////////////////////////////////////////////////////////////////////////////
// 画像のサイズが異なるエラー
void HTMLWriter::OutputImageSizeDiffrentError() {

    if ( !m_wfp )
        return;

    _ftprintf(m_wfp, _T("<H3>画像サイズが異なりますので比較できません</H3><BR>\n"));
}

