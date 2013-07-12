////////////////////////////////////////////////////////////////////////////
/** @file
    @brief  テスト用ライブラリ
    @author oniprog
*/
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AutoGuiTestSupport {

// コンソール画面を強制表示する
extern void ShowConsole();

// SDIプログラム用のプログラム終了
extern void ExitProgramSDI();

// 指定窓のキャプチャをとって，それを指定ファイルに保存する
extern void CaptureScreen(CWnd *pWnd, const std::wstring &szFilePath);

/// スリープ時間を秒単位で指定する．0.5秒等も指定可能
extern void Sleep(double dTime);


}; //namespace AutoGuiTestSupport
