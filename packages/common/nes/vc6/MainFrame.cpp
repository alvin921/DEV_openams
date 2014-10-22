//
// メインウインドウクラス
//
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <commdlg.h>
#include <imm.h>

#include <string>
using namespace std;

#include "VirtuaNESres.h"
#include "App.h"
#include "Pathlib.h"
#include "Plugin.h"

#include "Wnd.h"
#include "WndHook.h"
#include "MainFrame.h"
#include "Recent.h"
#include "Config.h"

#include "AboutDlg.h"
#include "RomInfoDlg.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#include "nes.h"
#include "rom.h"
#include "pad.h"

BOOL	CMainFrame::m_bKeyEnable = FALSE;

// メッセージマップ
WND_MESSAGE_BEGIN(CMainFrame)
// メッセージ
WND_ON_MESSAGE( WM_CREATE,	OnCreate )
WND_ON_MESSAGE( WM_CLOSE,	OnClose )
WND_ON_MESSAGE( WM_DESTROY,	OnDestroy )
WND_ON_MESSAGE( WM_GETMINMAXINFO, OnGetMinMaxInfo )
WND_ON_MESSAGE( WM_ACTIVATE,	OnActivate )
WND_ON_MESSAGE( WM_ACTIVATEAPP,	OnActivateApp )
WND_ON_MESSAGE( WM_ENABLE,	OnEnable )
WND_ON_MESSAGE( WM_ENTERMENULOOP, OnEnterMenuLoop )
WND_ON_MESSAGE( WM_EXITMENULOOP, OnExitMenuLoop )
WND_ON_MESSAGE( WM_SETFOCUS,	OnSetFocus )
WND_ON_MESSAGE( WM_KILLFOCUS,	OnKillFocus )
WND_ON_MESSAGE( WM_INITMENU,	OnInitMenu )
WND_ON_MESSAGE( WM_INITMENUPOPUP, OnInitMenuPopup )
WND_ON_MESSAGE( WM_PAINT,	OnPaint )
WND_ON_MESSAGE( WM_MENUCHAR,	OnMenuChar )
WND_ON_MESSAGE( WM_KEYDOWN,	OnKeyDown )
WND_ON_MESSAGE( WM_KEYUP, OnKeyUp )
WND_ON_MESSAGE( WM_SIZE,	OnSize )
WND_ON_MESSAGE( WM_SYSCOMMAND,	OnSysCommand )
WND_ON_MESSAGE( WM_EXITSIZEMOVE, OnExitSizeMove )


WND_ON_MESSAGE( WM_SETCURSOR,	OnSetCursor )
WND_ON_MESSAGE( WM_TIMER,	OnTimer )

WND_ON_MESSAGE( WM_COPYDATA, OnCopyData )

WND_ON_MESSAGE( WM_VNS_ERRORMSG, OnErrorMessage )

WND_ON_MESSAGE( WM_DROPFILES,	OnDropFiles )
WND_ON_MESSAGE( WM_VNS_COMMANDLINE, OnCommandLine )
WND_ON_MESSAGE( WM_VNS_LAUNCHERCMD, OnLauncherCommand )

// コマンド
WND_COMMAND_BEGIN()
WND_ON_COMMAND( ID_OPEN, OnFileOpen )
WND_ON_COMMAND( ID_CLOSE, OnFileClose )
WND_ON_COMMAND( ID_ROMINFO, OnRomInfo )
WND_ON_COMMAND_RANGE( ID_MRU_PATH0, ID_MRU_PATH9, OnRecentOpenPath )
WND_ON_COMMAND_RANGE( ID_MRU_FILE0, ID_MRU_FILE9, OnRecentOpen )
WND_ON_COMMAND( ID_EXIT, OnExit )

WND_ON_COMMAND( ID_HWRESET, OnEmuCommand )
WND_ON_COMMAND( ID_SWRESET, OnEmuCommand )
WND_ON_COMMAND( ID_PAUSE, OnEmuCommand )
WND_ON_COMMAND( ID_SNAPSHOT, OnEmuCommand )

WND_ON_COMMAND_RANGE( ID_QUICKLOAD_SLOT0, ID_QUICKLOAD_SLOT9, OnStateCommand2 )
WND_ON_COMMAND_RANGE( ID_QUICKSAVE_SLOT0, ID_QUICKSAVE_SLOT9, OnStateCommand2 )

WND_ON_COMMAND( ID_ABOUT, OnAbout )

WND_COMMAND_END()
WND_MESSAGE_END()

BOOL	CMainFrame::Create( HWND hWndParent )
{
	// NESオブジェクト
	Nes = NULL;

	// メンバ変数
	m_hMenu = NULL;

	WNDCLASSEX	wcl;
	ZEROMEMORY( &wcl, sizeof(wcl) );
	wcl.cbSize		= sizeof(wcl);
	wcl.lpszClassName	= "VirtuaNESwndclass";
	wcl.lpfnWndProc		= g_WndProc;
//	wcl.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcl.style		= CS_DBLCLKS;
	wcl.cbClsExtra		= wcl.cbWndExtra = 0;
	wcl.hInstance		= CApp::GetInstance();
	wcl.lpszMenuName	= NULL;
	wcl.hIcon		= 
	wcl.hIconSm		= ::LoadIcon( CApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON) );
	wcl.hCursor		= ::LoadCursor( NULL, IDC_ARROW );
	wcl.hbrBackground	= 0;

	if( !RegisterClassEx( &wcl ) ) {
//		DebugOut( "RegisterClassEx faild.\n" );
		return	FALSE;
	}

	HWND hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			VIRTUANES_WNDCLASS,
			VIRTUANES_CAPTION,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			hWndParent,
			NULL,
			CApp::GetInstance(),
			(LPVOID)this		// This を埋め込む為
		);
	if( !hWnd ) {
//		DebugOut( "CreateWindow faild.\n" );
		return	FALSE;
	}

	// メインウインドウとして登録
	CApp::SetHWnd( hWnd );

	// ステートスロット
	m_nStateSlot = 0;

	return	TRUE;
}

void	CMainFrame::Destroy()
{
	if( m_hWnd && IsWindow(m_hWnd) ) {
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
	}
}

BOOL	CMainFrame::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST ) {
//		return	TRUE;
	}
	return	FALSE;
}

WNDMSG	CMainFrame::OnCreate( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnCreate\n" );

	DirectDraw_Initial();
	// DirectSoundｵﾌﾟｼｮﾝの設定
	if( !DirectSound_Initial( Config.sound.nRate, Config.sound.nBits, Config.sound.nBufferSize ) ) {
		hResult = -1L;
		return	TRUE;
	}

	DirectInput_Initial();

	// メニューのロード
	m_hMenu = CApp::LoadMenu( IDR_MENU );
	CApp::SetMenu( m_hMenu );
	::SetMenu( m_hWnd, m_hMenu );

	// メニューにアクセラレータキー名の追加
	OnRebuildMenu();

	// ウインドウ位置の設定
	RECT	rc = Config.general.rcWindowPos;
	if( !((rc.right-rc.left) <= 0 || (rc.bottom-rc.top) <= 0) ) {
		if( (m_bZoomed = Config.general.bWindowZoom) ) {
			CApp::SetCmdShow( SW_SHOWMAXIMIZED );
		}
		// 保存されていたウインドウサイズにする
		m_WindowRect = rc;
		::SetWindowPos( m_hWnd, HWND_NOTOPMOST, rc.left, rc.top,
				rc.right-rc.left, rc.bottom-rc.top, 0 );
	} else {
		m_bZoomed = FALSE;
		// デフォルトサイズを調整する
		OnSetWindowSize();
	}


	// IME使用禁止:D
	::ImmAssociateContext( m_hWnd, NULL );

	// Drag&Dropの許可
	::DragAcceptFiles( m_hWnd, TRUE );

	// フラグ類
	m_bActivate = TRUE;
	m_bActivateApp = TRUE;
	m_bForcus = TRUE;
	m_bCursor = TRUE;
	m_bEnable = TRUE;
	m_bKeyEnable = TRUE;
	m_LastMovedTime = 0;

	m_uTimerID = ::SetTimer( m_hWnd, 0x0001, 1000, NULL );
//	m_uKeyTimerID = ::SetTimer( m_hWnd, 0x0100, 30, NULL );	// 30ms


#if	0
	// For Command Lines
	if( ::strlen( CApp::GetCmdLine() ) > 0 ) {
		LPSTR	pCmd = CApp::GetCmdLine();
		if( pCmd[0] == '"' ) {	// Shell execute!!
			ZEROMEMORY( m_szCommandLine, sizeof(m_szCommandLine) );
			memcpy( m_szCommandLine, pCmd+1, ::strlen(pCmd)-2 );
			m_szCommandLine[::strlen(m_szCommandLine)] = '\0';
		} else {
			strcpy( m_szCommandLine, pCmd );
		}
		::PostMessage( m_hWnd, WM_VNS_COMMANDLINE, 0, 0L );
	}
#endif
	return	FALSE;
}

WNDMSG	CMainFrame::OnClose( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnClose\n" );

	// エミュレーション終了
	Stop();
	NES_Destroy( Nes );
	Nes = NULL;

	// タイマーの終了
	::KillTimer( m_hWnd, m_uTimerID );
//	::KillTimer( m_hWnd, m_uKeyTimerID );
	m_uTimerID = 0;

	// ウインドウ破棄
	::DestroyWindow( m_hWnd );

	// ウインドウ位置の保存
	Config.general.bWindowZoom = m_bZoomed;
	Config.general.rcWindowPos = m_WindowRect;

	::DragAcceptFiles( m_hWnd, FALSE );

	return	FALSE;
}

WNDMSG	CMainFrame::OnDestroy( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnDestroy\n" );

	::PostQuitMessage( 0 );
	return	FALSE;
}

WNDMSG	CMainFrame::OnGetMinMaxInfo( WNDMSGPARAM )
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	// 最小サイズだけ
	lpMMI->ptMinTrackSize.x = 128;
	lpMMI->ptMinTrackSize.y = 128;
//	lpMMI->ptMaxTrackSize.x = 65535;
//	lpMMI->ptMaxTrackSize.y = 65535;

	return	TRUE;
}

WNDMSG	CMainFrame::OnActivate( WNDMSGPARAM )
{
//DebugOut( "WA_ACTIVATE: Minimized:%s  Prev:%08X  This:%08X\n", HIWORD(wParam)?"TRUE":"FALSE", lParam, m_hWnd );

	if( HIWORD(wParam) || (LOWORD(wParam) == WA_INACTIVE && !lParam) ) {
		if( m_bActivate ) {
//		DebugOut( "Inactivate.\n" );
			m_bActivate = FALSE;
			// バックグラウンド動作ONかネットプレイ中はポーズしない
			if( !(Config.emulator.bBackground) ) {
				Pause();
			}
		}
	} else if( !lParam ) {
//		DebugOut( "Activate.\n" );
		m_bActivate = TRUE;
		// バックグラウンド動作ONかネットプレイ中はレジュームしない
		if( !(Config.emulator.bBackground) ) {
			Resume();
		}
	}
	return	FALSE;
}

WNDMSG	CMainFrame::OnActivateApp( WNDMSGPARAM )
{
//DebugOut( "wParam:%08X  lParam:%08X\n", wParam, lParam );

	if( (BOOL)wParam ) {
//		DebugOut( "CMainFrame::OnActivateApp:Active\n" );
		m_bActivateApp = TRUE;
		if( !m_bActivate ) {
			m_bActivate = TRUE;
			// バックグラウンド動作ONかネットプレイ中はレジュームしない
			if( !Config.emulator.bBackground ) {
				Resume();
			}
		}
	} else {
//		DebugOut( "CMainFrame::OnActivateApp:Inactive\n" );
		m_bActivateApp = FALSE;
		if( m_bActivate ) {
			m_bActivate = FALSE;
			// バックグラウンド動作ONかネットプレイ中はポーズしない
			if( !Config.emulator.bBackground ) {
				Pause();
			}
		}
	}
	return	FALSE;
}

int	CMainFrame::ProcessKey(int nChar)
{
	//DebugOut( "wParam:%08X  lParam:%08X\n", wParam, lParam );
		int bit = 0;
	
		switch(nChar){
			case 'W': case 'w': bit = NES_KEY_UP; break;
			//case 'X': case 'x'	: bit = NES_KEY_DOWN; break;
			case 'A': case 'a'	: bit = NES_KEY_LEFT; break;
			case 'D': case 'd'	: bit = NES_KEY_RIGHT; break;
			case 'K': case 'k'	: bit = NES_KEY_A; break;
			case 'L': case 'l'	: bit = NES_KEY_B; break;

			case VK_UP		: bit = NES_KEY_UP; break;
			case VK_DOWN	: bit = NES_KEY_DOWN; break;
			case VK_LEFT		: bit = NES_KEY_LEFT; break;
			case VK_RIGHT	: bit = NES_KEY_RIGHT; break;
			case 'Z': case 'z'	: bit = NES_KEY_A; break;
			case 'X': case 'x'	: bit = NES_KEY_B; break;
			
			case VK_SPACE	: bit = NES_KEY_SELECT; break;
			case VK_RETURN	: bit = NES_KEY_START; break;

			case VK_PRIOR	: bit = NES_KEY_PRIOR; break;
			case VK_NEXT	: bit = NES_KEY_NEXT; break;
		}
	
		return bit;
}


WNDMSG	CMainFrame::OnKeyDown( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnKeyDown\n" );

	m_nKeyBit = ProcessKey(wParam);
	m_nKeyTimerID = 0;
	if(m_nKeyBit){
		m_nKeyTimerID = ::SetTimer( m_hWnd, 0x0002, 20, NULL );
		NES_CommandReq(EV_KEY, m_nKeyBit, 0);
	}
	return	FALSE;
}

WNDMSG	CMainFrame::OnKeyUp( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnKeyDown\n" );

	if(m_nKeyBit){
		::KillTimer( m_hWnd, m_nKeyTimerID );
		m_nKeyTimerID = 0;
		m_nKeyBit = 0;
		//Emu.EventParam(CEmuThread::EV_KEY, m_nKeyBit);
	}
	return	FALSE;
}

WNDMSG	CMainFrame::OnEnable( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnEnable = %s\n", (BOOL)wParam?"TRUE":"FALSE" );

	if( (BOOL)wParam )
		Resume();
	else
		Pause();

	m_bEnable    = (BOOL)wParam;
	m_bKeyEnable = (BOOL)wParam;

	return	FALSE;
}

WNDMSG	CMainFrame::OnEnterMenuLoop( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnEnterMenuLoop\n" );
	m_bKeyEnable = FALSE;
	Pause();
	return	FALSE;
}

WNDMSG	CMainFrame::OnExitMenuLoop( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnExitMenuLoop\n" );
	m_bKeyEnable = TRUE;
	Resume();
	return	FALSE;
}

WNDMSG	CMainFrame::OnSetFocus( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnSetFocus\n" );
	m_bForcus = TRUE;
	return	FALSE;
}

WNDMSG	CMainFrame::OnKillFocus( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnKillFocus\n" );
	m_bForcus = FALSE;
	return	FALSE;
}

WNDMSG	CMainFrame::OnInitMenu( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnInitMenu\n" );
	CRecent::UpdateMenu( CApp::GetMenu() );
	DrawMenuBar( m_hWnd );
	return	FALSE;
}

WNDMSG	CMainFrame::OnInitMenuPopup( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnInitMenuPopup\n" );
	// System menu
	if( (BOOL)HIWORD(lParam) )
		return	FALSE;

//	DebugOut( "SubMenu=%08X uPos=%d bSys=%d\n", wParam, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam) );

	HMENU	hMenu = (HMENU)wParam;

	INT	MenuCount = ::GetMenuItemCount( hMenu );
	for( INT i = 0; i < MenuCount; i++ ) {
		OnUpdateMenu( hMenu, ::GetMenuItemID( hMenu, i ) );
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnPaint( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnPaint\n" );
	HDC	hDC;
	PAINTSTRUCT	ps;
	hDC = ::BeginPaint( m_hWnd, &ps );
	if( !NES_IsRunning() ) {
		RECT	rc;
		::GetClientRect( m_hWnd, &rc );
		::SetBkColor( hDC, 0x00000000 );
		::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
	} else if( NES_IsPausing() ) {
		DirectDraw_Paint();
	}
	::EndPaint( m_hWnd, &ps );

	return	TRUE;
}

WNDMSG	CMainFrame::OnMenuChar( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnMenuChar\n" );
	// キンキンうるさいから閉じる…と嘘をつく
	hResult = MAKELONG(0,MNC_CLOSE);
	return	TRUE;
}

WNDMSG	CMainFrame::OnSize( WNDMSGPARAM )
{
	// 最大化表示等
	// WM_SYSCOMMANDでも良いのだが，キャプションをダブルクリックしたときに来ないので...
	switch( wParam ) {
		case	SIZE_MAXIMIZED:
			m_bZoomed = TRUE;
			{
			WINDOWPLACEMENT wpl;
			::GetWindowPlacement( m_hWnd, &wpl );
			m_WindowRect = wpl.rcNormalPosition;	// 元の位置をコピー
			}
			break;
		case	SIZE_RESTORED:
			m_bZoomed = FALSE;
			break;
		default:
			break;
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnSysCommand( WNDMSGPARAM )
{
	// エミュレーション中のみ
	if( NES_IsRunning() ) {
		// スクリーンセーバーの起動の抑止
		if( wParam == SC_SCREENSAVE ) {
//			DebugOut( "CMainFrame::OnSysCommand  SC_SCREENSAVE\n" );
			hResult = 1L;
			return	TRUE;
		}
		// ディスプレイ電源オフの抑止
		if( wParam == SC_MONITORPOWER ) {
//			DebugOut( "CMainFrame::OnSysCommand  SC_MONITORPOWER\n" );
			hResult = 1L;
			return	TRUE;
		}
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnExitSizeMove( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnExitSizeMove\n" );

	::GetWindowRect( m_hWnd, &m_WindowRect );

	return	FALSE;
}

WNDMSG	CMainFrame::OnSetCursor( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnSetCursor\n" );
	if( LOWORD(lParam) == HTCLIENT ) {
		if( NES_IsRunning() ) {
//			if( !Emu.NES_IsPausing() && !Emu.IsEmuPausing() ) {
			if( !NES_IsPausing() ) {
				m_bCursor = FALSE;
				m_LastMovedTime = tick_current();
			} else {
				m_bCursor = TRUE;
			}
		} else {
			m_bCursor = TRUE;
		}
	} else {
		m_bCursor = TRUE;
	}

	return	FALSE;
}

void	CMainFrame::OnShowCursor( BOOL bFlag )
{
	if( !bFlag ) {
		if( m_bCursor ) {
			while( ::ShowCursor( FALSE ) >= 0 );
			m_bCursor = FALSE;
		}
	} else {
		if( !m_bCursor ) {
			while( ::ShowCursor( TRUE ) < 0 );
			m_bCursor = TRUE;
		}
	}
}

WNDMSG	CMainFrame::OnTimer( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnTimer\n" );
//	DebugOut( "CMainFrame::OnTimer bCursor=%s bForcus=%s\n", m_bCursor?"TRUE":"FALSE", m_bForcus?"TRUE":"FALSE" );

	if( wParam == 0x0001 ) {
	// マウスカーソル用タイマー
		RECT	rc;
		POINT	pt;
		::GetWindowRect( m_hWnd, &rc );
		::GetCursorPos( &pt );

		if( !m_bCursor && m_bForcus && pt.x >= rc.left && pt.x <= rc.right && pt.y >= rc.top && pt.y <= rc.bottom ) {
			if( NES_IsRunning() && !NES_IsPausing() ) {
				if( (tick_current()-m_LastMovedTime) > 1500 ) {
					::SetCursor( NULL );
				}
			}
		}
	} else if( wParam == 0x0002 ) {
		if(m_nKeyBit)
			NES_CommandReq(EV_KEY, m_nKeyBit, 0);
	} else if( wParam == 0x0100 ) {
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnCopyData( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnCopyData\n" );

	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

	OnEmulationStart( (LPSTR)pcds->lpData, FALSE );

	return	TRUE;
}

WNDMSG	CMainFrame::OnErrorMessage( WNDMSGPARAM )
{
//	DebugOut( "CMainFrame::OnErrorMessage\n" );

	// エミュレーション終了
	Stop();
	NES_Destroy( Nes );
	Nes = NULL;

	// キャプションを変える
	::SetWindowText( m_hWnd, VIRTUANES_CAPTION );

	::MessageBox( m_hWnd, (LPCTSTR)lParam, "ERROR", MB_ICONERROR|MB_OK );
	::InvalidateRect( m_hWnd, NULL, TRUE );

	return	TRUE;
}

WNDCMD	CMainFrame::OnExit( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnExit\n" );

	::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
}

WNDCMD	CMainFrame::OnHelp( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnHelp\n" );

	string	sHelp = CPathlib::MakePath( CApp::GetModulePath(), "virtuanes.chm" );

	// 面倒なんでShellExecuteで代用
	::ShellExecute( HWND_DESKTOP, "open", sHelp.c_str(), NULL, NULL, SW_SHOWNORMAL );
//	::HtmlHelp( m_hWnd, "virtuanes.chm", HH_DISPLAY_TOPIC, NULL ); 
}

WNDCMD	CMainFrame::OnAbout( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnAbout\n" );
	CAboutDlg dlg;
	dlg.DoModal( m_hWnd );
}

WNDCMD	CMainFrame::OnFileOpen( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnFileOpen\n" );

	if( NES_IsRunning() ) {
		Pause();
	}

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	ZEROMEMORY( szFile, sizeof(szFile) );
	ZEROMEMORY( &ofn, sizeof(ofn) );

	string	pathstr;
	if( Config.path.bRomPath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szRomPath );
	} else {
		pathstr = CApp::GetModulePath();
	}

	CHAR	szTitle[256];
	CApp::LoadString( IDS_UI_OPENROM, szTitle, sizeof(szTitle) );
	ofn.lpstrTitle = szTitle;

	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = m_hWnd;
	ofn.lpstrFile       = szFile;
	ofn.nMaxFile        = sizeof(szFile);
	ofn.lpstrFilter     = "All Support Types\0*.nes;*.fds;*.nsf;*.lzh;*.zip;*.rar;*.cab\0"
			      "NES ROM (*.nes)\0*.nes\0Disk Image (*.fds)\0*.fds\0"
			      "NES Music File (*.nsf)\0*.nsf\0Archive File\0*.lzh;*.zip;*.rar;*.cab\0";
	ofn.nFilterIndex    = 1;
	ofn.Flags           = OFN_READONLY|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = pathstr.c_str();

	if( ::GetOpenFileName( &ofn ) ) {
		OnEmulationStart( szFile, FALSE );
	}
	while( NES_IsPausing() ) {
		Resume();
	}
}

WNDCMD	CMainFrame::OnFileClose( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnFileClose\n" );

	if( NES_IsRunning() ) {
		Stop();
		NES_Destroy( Nes );
		Nes = NULL;

		// キャプションを変える
		::SetWindowText( m_hWnd, VIRTUANES_CAPTION );

		// 再描画
		::InvalidateRect( m_hWnd, NULL, TRUE );
	}
}

WNDMSG	CMainFrame::OnCommandLine( WNDMSGPARAM )
{
	OnEmulationStart( (LPSTR)lParam, FALSE );

	return	TRUE;
}

WNDMSG	CMainFrame::OnLauncherCommand( WNDMSGPARAM )
{
	::SetForegroundWindow( m_hWnd );

	OnEmulationStart( (LPSTR)lParam, FALSE );

	return	TRUE;
}

WNDCMD	CMainFrame::OnRecentOpen( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnRecentOpen ID=%d\n", uID-ID_MRU_FILE0 );
//	DebugOut( "Fname: \"%s\"\n", CRecent::GetName( (INT)uID-ID_MRU_FILE0 ) );

	OnEmulationStart( CRecent::GetName( (INT)uID-ID_MRU_FILE0 ), FALSE );
}

WNDCMD	CMainFrame::OnRecentOpenPath( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnRecentOpenPath ID=%d\n", uID-ID_MRU_FILE0 );
//	DebugOut( "Fname: \"%s\"\n", CRecent::GetPath( (INT)uID-ID_MRU_FILE0 ) );

	if( NES_IsRunning() ) {
		Pause();
	}

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	ZEROMEMORY( szFile, sizeof(szFile) );
	ZEROMEMORY( &ofn, sizeof(ofn) );

	CHAR	szTitle[256];
	CApp::LoadString( IDS_UI_OPENROM, szTitle, sizeof(szTitle) );
	ofn.lpstrTitle = szTitle;

	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = m_hWnd;
	ofn.lpstrFile       = szFile;
	ofn.nMaxFile        = sizeof(szFile);
	ofn.lpstrFilter     = "All Support Types\0*.nes;*.fds;*.nsf;*.lzh;*.zip;*.rar;*.cab\0"
			      "NES ROM (*.nes)\0*.nes\0Disk Image (*.fds)\0*.fds\0"
			      "NES Music File (*.nsf)\0*.nsf\0Archive File\0*.lzh;*.zip;*.rar;*.cab\0";
	ofn.nFilterIndex    = 1;
	ofn.Flags           = OFN_READONLY|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = CRecent::GetPath( (INT)uID-ID_MRU_PATH0 );


	if( ::GetOpenFileName( &ofn ) ) {
		OnEmulationStart( szFile, FALSE );
	}
	while( NES_IsPausing() ) {
		Resume();
	}
}

WNDMSG	CMainFrame::OnDropFiles( WNDMSGPARAM )
{
	DebugOut( "CMainFrame::OnDropFiles\n" );
	::SetForegroundWindow( m_hWnd );

	CHAR szFile[_MAX_PATH];
	::DragQueryFile( (HDROP)wParam, 0, szFile, _MAX_PATH );
	::DragFinish( (HDROP)wParam );

	INT	ret;
	if( (ret = ROM_IsRomFile( szFile )) >= 0 ) {
DebugOut( "ROMファイル CHK=%d\n", ret );
		if( ret == IDS_ERROR_ILLEGALHEADER ) {
			if( ::MessageBox( m_hWnd, CApp::GetErrorString(ret), "VirtuaNES", MB_ICONWARNING|MB_YESNO ) != IDYES )
				return	TRUE;

			OnEmulationStart( szFile, TRUE );
			return	TRUE;
		} else if( ret == 0 ) {
			OnEmulationStart( szFile, TRUE );
			return	TRUE;
		}
	}

	if( NES_IsRunning()) {
		if( (ret = NES_IsStateFile( szFile, Nes->rom )) >= 0 ) {
DebugOut( "ステートファイル CHK=%d\n", ret );
			if( ret == IDS_ERROR_ILLEGALSTATECRC ) {
				if( Config.emulator.bCrcCheck ) {
					if( ::MessageBox( m_hWnd, CApp::GetErrorString(ret), "VirtuaNES", MB_ICONWARNING|MB_YESNO ) != IDYES )
						return	TRUE;
				}
			}
			NES_CommandReq( NESCMD_STATE_LOAD, (INT)szFile, -1 );
		}
	}

	return	TRUE;
}

void	CMainFrame::OnEmulationStart( LPCSTR szFile, BOOL bChecked )
{

	// パターンビュア終了
//	m_PatternView.Destroy();
//	m_NameTableView.Destroy();
//	m_PaletteView.Destroy();
//	m_MemoryView.Destroy();

	// エミュレーション終了
//	Emu.Stop();
//	DELETEPTR( Nes );

	try {
		if( !bChecked ) {
			INT	ret;
			if( (ret = ROM_IsRomFile( szFile )) != 0 ) {
				// 致命的なエラー
				if( ret == ROM_ERR_OPEN ) {
					// xxx ファイルを開けません
					::wsprintf( szErrorString, CApp::GetErrorString(ret), szFile );
					throw	szErrorString;
				}
				if( ret == IDS_ERROR_READ ) {
					throw	CApp::GetErrorString(ret);
				}
				if( ret == IDS_ERROR_UNSUPPORTFORMAT ) {
					throw	CApp::GetErrorString(ret);
				}

				// YES/NOチェック
				if( ret == IDS_ERROR_ILLEGALHEADER ) {
					if( ::MessageBox( m_hWnd, CApp::GetErrorString(ret), "VirtuaNES", MB_ICONWARNING|MB_YESNO ) != IDYES )
						return;
				}
//			} else {
//				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
			}
		}

		// エミュレーション終了
		Stop();
		NES_Destroy( Nes );
		Nes = NULL;

		if( (Nes = NES_New(szFile)) ) {
			CRecent::Add( szFile );

			// キャプションを変える
			{
			string	str = VIRTUANES_CAPTION;
				str = str + " - " + ROM_GetRomName(Nes->rom);
				::SetWindowText( m_hWnd, str.c_str() );
			}

			// エミュレーションスレッドスタート
			Start( m_hWnd, Nes );
		} else {
			// xxx ファイルを開けません
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, szFile );
			throw	szErrorString;
		}
	} catch( CHAR* str ) {
		strcpy( szErrorString, str );
		PostMessage( m_hWnd, WM_VNS_ERRORMSG, 0, (LPARAM)szErrorString );
	}
}

WNDCMD	CMainFrame::OnRomInfo( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnRomInfo\n" );

	if( !NES_IsRunning() || !Nes )
		return;

	CRomInfoDlg dlg;

	// メンバの設定
	strcpy( dlg.m_szName, ROM_GetRomName(Nes->rom) );
	dlg.m_nMapper = ROM_GetMapperNo(Nes->rom);
	dlg.m_nPRG    = ROM_GetPROM_SIZE(Nes->rom);
	dlg.m_nCHR    = ROM_GetVROM_SIZE(Nes->rom);
	dlg.m_bMirror  = ROM_IsVMIRROR(Nes->rom);
	dlg.m_bSram    = ROM_IsSAVERAM(Nes->rom);
	dlg.m_b4Screen = ROM_Is4SCREEN(Nes->rom);
	dlg.m_bTrainer = ROM_IsTRAINER(Nes->rom);
	dlg.m_bVSUnisystem = ROM_IsVSUNISYSTEM(Nes->rom);

	if( ROM_GetMapperNo(Nes->rom) < 256 && ROM_GetMapperNo(Nes->rom) != 20 ) {
		dlg.m_dwCRC    = ROM_GetPROM_CRC(Nes->rom);
		dlg.m_dwCRCALL = ROM_GetROM_CRC(Nes->rom);
		dlg.m_dwCRCCHR = ROM_GetVROM_CRC(Nes->rom);
	}

	dlg.DoModal( m_hWnd );
}

WNDCMD	CMainFrame::OnEmuCommand( WNDCMDPARAM )
{
//	DebugOut( "CMainFrame::OnEmuCommand %d\n", uID );

	switch( uID ) {
		case	ID_HWRESET:
			NES_CommandReq( EV_HWRESET, 0, 0 );
			break;
		case	ID_SWRESET:
			NES_CommandReq( EV_SWRESET, 0, 0 );
			break;
		case	ID_PAUSE:
			NES_CommandReq( NESCMD_EMUPAUSE, 0, 0 );
			break;

		case	ID_SNAPSHOT:
			NES_CommandReq( NESCMD_SNAPSHOT, 0, 0 );
			break;

		default:
			break;
	}
}

WNDCMD	CMainFrame::OnStateCommand2( WNDCMDPARAM )
{
	if( !NES_IsRunning() )
		return;

	BOOL	bLoad = FALSE;
	INT	slot = 0;
	if( uID >= ID_QUICKLOAD_SLOT0 && uID <= ID_QUICKLOAD_SLOT9 ) {
		bLoad = TRUE;
		slot = uID - ID_QUICKLOAD_SLOT0;
	} else {
		bLoad = FALSE;
		slot = uID - ID_QUICKSAVE_SLOT0;
	}

	CHAR	st[16];
	::wsprintf( st, "st%1X", slot );

	string	pathstr, tempstr;
	if( Config.path.bStatePath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szStatePath );
		::CreateDirectory( pathstr.c_str(), NULL );
	DebugOut( "Path: %s\n", pathstr.c_str() );
	} else {
		pathstr = ROM_GetRomPath(Nes->rom);
	}
	tempstr = CPathlib::MakePathExt( pathstr.c_str(), ROM_GetRomName(Nes->rom), st );
	DebugOut( "Path: %s\n", tempstr.c_str() );

	if( bLoad ) {
		INT	ret;
		if( (ret = NES_IsStateFile( tempstr.c_str(), Nes->rom )) >= 0 ) {
			if( ret == IDS_ERROR_ILLEGALSTATECRC ) {
				if( Config.emulator.bCrcCheck ) {
					if( ::MessageBox( m_hWnd, CApp::GetErrorString(ret), "VirtuaNES", MB_ICONWARNING|MB_YESNO ) != IDYES )
						return;
				}
			}
		}
		NES_CommandReq( NESCMD_STATE_LOAD, (INT)tempstr.c_str(), slot );
	}
	if( !bLoad ) {
		NES_CommandReq( NESCMD_STATE_SAVE, (INT)tempstr.c_str(), slot );
	}
}

void	CMainFrame::OnSetWindowSize()
{
	// ウインドウモードの時だけ
	if( Config.general.bScreenMode )
		return;

	LONG	width, height;

	if( !Config.graphics.bAspect )  width  = SCREEN_WIDTH;
	else				width  = (LONG)((SCREEN_WIDTH)*1.25);
	if( !Config.graphics.bAllLine ) height = SCREEN_HEIGHT-16;
	else				height = SCREEN_HEIGHT;

	width  *= Config.general.nScreenZoom+1;
	height *= Config.general.nScreenZoom+1;

	RECT	rcW, rcC;
	::GetWindowRect( m_hWnd, &rcW );
	::GetClientRect( m_hWnd, &rcC );

	rcW.right  += width -(rcC.right-rcC.left);
	rcW.bottom += height-(rcC.bottom-rcC.top);
	::SetWindowPos( m_hWnd, HWND_NOTOPMOST, rcW.left, rcW.top,
			rcW.right-rcW.left, rcW.bottom-rcW.top, SWP_NOZORDER );

	// メニューが折り返されると縦が小さくなる事があるので再修正
	::GetClientRect( m_hWnd, &rcC );
	if( (rcC.bottom-rcC.top) != height ) {
		::GetWindowRect( m_hWnd, &rcW );
		::GetClientRect( m_hWnd, &rcC );

		rcW.right  += width -(rcC.right-rcC.left);
		rcW.bottom += height-(rcC.bottom-rcC.top);
		::SetWindowPos( m_hWnd, HWND_NOTOPMOST, rcW.left, rcW.top,
				rcW.right-rcW.left, rcW.bottom-rcW.top, SWP_NOZORDER );
	}


	// 最大表示時にサイズを変更した場合の最大表示の解除及びボタンの再描画
	LONG	style = ::GetWindowLong( m_hWnd, GWL_STYLE );
	style &= ~WS_MAXIMIZE;
	::SetWindowLong( m_hWnd, GWL_STYLE, style );
	::RedrawWindow( m_hWnd, NULL, NULL, RDW_FRAME|RDW_INVALIDATE );

	// 位置保存しなおし
	m_bZoomed = FALSE;
	::GetWindowRect( m_hWnd, &m_WindowRect );
}

void	CMainFrame::OnRebuildMenu()
{
}

void	CMainFrame::OnUpdateMenu( HMENU hMenu, UINT uID )
{
	BOOL	bEnable = FALSE;
	BOOL	bCheck = FALSE;
	BOOL	bEmu = NES_IsRunning();
	BOOL	bScn = Config.general.bScreenMode;

	switch( uID ) {
		case	ID_CLOSE:
		case	ID_ROMINFO:
			::EnableMenuItem( hMenu, uID, MF_BYCOMMAND|(bEmu?MF_ENABLED:MF_GRAYED) );
			break;

		case	ID_HWRESET:
		case	ID_SWRESET:
		case	ID_PAUSE:
			::EnableMenuItem( hMenu, uID, MF_BYCOMMAND|(bEmu?MF_ENABLED:MF_GRAYED) );
			break;

		case	ID_QUICKLOAD_SLOT0:	case	ID_QUICKLOAD_SLOT1:
		case	ID_QUICKLOAD_SLOT2:	case	ID_QUICKLOAD_SLOT3:
		case	ID_QUICKLOAD_SLOT4:	case	ID_QUICKLOAD_SLOT5:
		case	ID_QUICKLOAD_SLOT6:	case	ID_QUICKLOAD_SLOT7:
		case	ID_QUICKLOAD_SLOT8:	case	ID_QUICKLOAD_SLOT9:
		case	ID_QUICKSAVE_SLOT0:	case	ID_QUICKSAVE_SLOT1:
		case	ID_QUICKSAVE_SLOT2:	case	ID_QUICKSAVE_SLOT3:
		case	ID_QUICKSAVE_SLOT4:	case	ID_QUICKSAVE_SLOT5:
		case	ID_QUICKSAVE_SLOT6:	case	ID_QUICKSAVE_SLOT7:
		case	ID_QUICKSAVE_SLOT8:	case	ID_QUICKSAVE_SLOT9:
			// 日時表示(結構面倒ｗ)
			{
			CHAR	szMenuString[256];
			CHAR*	pToken;
			const UCHAR seps[] = " \t\0";	// セパレータ

			::GetMenuString( m_hMenu, uID, szMenuString, 256, MF_BYCOMMAND );

			if( (pToken = (CHAR*)_mbstok( (UCHAR*)szMenuString, seps )) ) {
				string	str = pToken;

				if( NES_IsRunning() && Nes ) {
					CHAR	temp[256];
					if( uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9 )
						::wsprintf( temp, "st%1X", uID-ID_STATE_SLOT0 );
					else if( uID >= ID_QUICKLOAD_SLOT0 && uID <= ID_QUICKLOAD_SLOT9 )
						::wsprintf( temp, "st%1X", uID-ID_QUICKLOAD_SLOT0 );
					else
						::wsprintf( temp, "st%1X", uID-ID_QUICKSAVE_SLOT0 );

					string	pathstr, tempstr;
					if( Config.path.bStatePath ) {
						pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szStatePath );
						::CreateDirectory( pathstr.c_str(), NULL );
					} else {
						pathstr = ROM_GetRomPath(Nes->rom);
					}
					tempstr = CPathlib::MakePathExt( pathstr.c_str(), ROM_GetRomName(Nes->rom), temp );

					HANDLE	hFile = ::CreateFile( tempstr.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
					if( hFile != INVALID_HANDLE_VALUE ) {
						BY_HANDLE_FILE_INFORMATION fi;
						FILETIME	filetime;
						SYSTEMTIME	systime;
						::GetFileInformationByHandle( hFile, &fi );
						::FileTimeToLocalFileTime( &fi.ftLastWriteTime, &filetime );
						::FileTimeToSystemTime( &filetime, &systime );
						::wsprintf( temp,"%04d/%02d/%02d %02d:%02d:%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond );
						::CloseHandle( hFile );

						str = str + " ";
						str = str + temp;
					} else {
						str = str + " not exist";
					}
				}

				::ModifyMenu( m_hMenu, uID, MF_BYCOMMAND|MF_STRING, uID, str.c_str() );
			}

			if( !(uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9) ) {
				::EnableMenuItem( hMenu, uID, MF_BYCOMMAND|(bEmu?MF_ENABLED:MF_GRAYED) );
			}

			if( uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9 ) {
				::CheckMenuItem( hMenu, uID, MF_BYCOMMAND|(m_nStateSlot==(INT)(uID-ID_STATE_SLOT0)?MF_CHECKED:MF_UNCHECKED) );
			}
			}
			break;

		case	ID_SNAPSHOT:
			::EnableMenuItem( hMenu, uID, MF_BYCOMMAND|(bEmu?MF_ENABLED:MF_GRAYED) );
			break;

		default:
			break;
	}
}

