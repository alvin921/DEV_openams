//
// メインウインドウクラス
//
#ifndef	__CMAINFRAME_INCLUDED__
#define	__CMAINFRAME_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
using namespace std;

#include "Wnd.h"
#include "EmuThread.h"

class	CMainFrame : public CWnd
{
public:
	// Override from CWnd
	BOOL	Create( HWND hWndParent );
	void	Destroy();

	// Override from CWnd
	BOOL	PreTranslateMessage( MSG* pMsg );

protected:
	void	OnEmulationStart( LPCSTR szFile, BOOL bChecked );

	void	OnShowCursor( BOOL bFlag );
	void	OnSetWindowSize();
	void	OnRebuildMenu();
	void	OnUpdateMenu( HMENU hMenu, UINT uID );

	// Message map
	WND_MESSAGE_MAP()
	WNDMSG		OnCreate( WNDMSGPARAM );
	WNDMSG		OnClose( WNDMSGPARAM );
	WNDMSG		OnDestroy( WNDMSGPARAM );
	WNDMSG		OnActivate( WNDMSGPARAM );
	WNDMSG		OnActivateApp( WNDMSGPARAM );
	WNDMSG		OnEnable( WNDMSGPARAM );
	WNDMSG		OnEnterMenuLoop( WNDMSGPARAM );
	WNDMSG		OnExitMenuLoop( WNDMSGPARAM );
	WNDMSG		OnSetFocus( WNDMSGPARAM );
	WNDMSG		OnKillFocus( WNDMSGPARAM );
	WNDMSG		OnInitMenu( WNDMSGPARAM );
	WNDMSG		OnInitMenuPopup( WNDMSGPARAM );
	WNDMSG		OnGetMinMaxInfo( WNDMSGPARAM );
	WNDMSG		OnPaint( WNDMSGPARAM );
	WNDMSG		OnMenuChar( WNDMSGPARAM );
	WNDMSG		OnKeyDown( WNDMSGPARAM );
	WNDMSG		OnKeyUp( WNDMSGPARAM );
	WNDMSG		OnSize( WNDMSGPARAM );
	WNDMSG		OnSysCommand( WNDMSGPARAM );
	WNDMSG		OnExitSizeMove( WNDMSGPARAM );

	WNDMSG		OnSetCursor( WNDMSGPARAM );
	WNDMSG		OnTimer( WNDMSGPARAM );

	WNDMSG		OnCopyData( WNDMSGPARAM );

	WNDMSG		OnErrorMessage( WNDMSGPARAM );		// Private message

	WNDMSG		OnDropFiles( WNDMSGPARAM );
	WNDMSG		OnCommandLine( WNDMSGPARAM );		// Private message
	WNDMSG		OnLauncherCommand( WNDMSGPARAM );	// Private message

	WNDCMD		OnExit( WNDCMDPARAM );
	WNDCMD		OnHelp( WNDCMDPARAM );
	WNDCMD		OnAbout( WNDCMDPARAM );
	WNDCMD		OnFileOpen( WNDCMDPARAM );
	WNDCMD		OnFileClose( WNDCMDPARAM );
	WNDCMD		OnRecentOpen( WNDCMDPARAM );
	WNDCMD		OnRecentOpenPath( WNDCMDPARAM );
	WNDCMD		OnRomInfo( WNDCMDPARAM );

	WNDCMD		OnEmuCommand( WNDCMDPARAM );

	WNDCMD		OnStateCommand( WNDCMDPARAM );
	WNDCMD		OnStateCommand2( WNDCMDPARAM );	// For QuickLoad/Save

	int ProcessKey(int nChar);

	// Command line
	CHAR	m_szCommandLine[_MAX_PATH];

	// Window activate flags
	BOOL	m_bActivate;
	BOOL	m_bActivateApp;
	BOOL	m_bForcus;

	BOOL	m_bEnable;
	static	BOOL	m_bKeyEnable;

	// Cursor
	BOOL	m_bCursor;
	UINT	m_uTimerID;
	DWORD	m_LastMovedTime;

	// Window Position
	BOOL	m_bZoomed;
	RECT	m_WindowRect;
	WINDOWPLACEMENT	m_WindowPlacement;

	// Emulator
	NES*		Nes;
//	CEmuThread	Emu;

	// State slot
	INT	m_nStateSlot;

	UINT	m_nKeyTimerID;
	DWORD	m_nKeyBit;

private:
};

#endif	// !__CMAINFRAME_INCLUDED__

