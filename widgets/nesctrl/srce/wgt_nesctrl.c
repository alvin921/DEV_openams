	
/*=============================================================================
	
$DateTime: 2010/06/06 16:41:00 $
$Author: Wang MinGang $

					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when		 who	 what, where, why
----------	 ---	 ----------------------------------------------------------
=============================================================================*/





/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	
#define DLL_EXPORT_nesctrl

#include "wgt_nesctrl.h"
#include "sys_resid.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef	PARENT_CLASS
#define	PARENT_CLASS	(&class_widget)

#undef	CURRENT_CLASS
#define	CURRENT_CLASS	(&class_nesctrl)

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_TaskID		tid;
	gu8			state;
	gs16 		m_nPauseCount;
}t_NESCtrl;


//2 NESTask
#include "nes.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#include "widget.h"

error_t 	NesOnMessage(gpointer data, t_AmosMsg *pmsg)
{
	switch(pmsg->evt){
		case EVT_APP_START:
			{
				DirectDraw_Initial((t_HWidget)pmsg->lparam);
				DirectSound_Initial( Config.sound.nRate, Config.sound.nBits, Config.sound.nBufferSize ) ;
				DirectInput_Initial();

				app_set_data(NULL);
			}
			break;
		case EVT_APP_END:
			if(data){
				NES_Command((NES*)data, NESCMD_STOP, NULL);
			}
			DirectDraw_Release();
			DirectSound_Release();
			break;
		case EVT_USER:
			NES_Command((NES*)data, pmsg->sparam, (t_NESCmdParam*)pmsg->lparam);
			break;

		default:
			break;
	}
	return SUCCESS;
}

gu32	NesOnIdle(void *data)
{
	int ret = 0;
	char errmsg[256];
	TRY(errmsg, sizeof(errmsg)){
		ret = NES_Process((NES*)data);
	}CATCH{
		//... post message the main window for
		g_printf("NES error: <%s>", errmsg);
	}
	TRYEND
	return ret;
}

/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__nesctrl_OnCreate(t_NESCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	{
		t_TaskID tid;
		tid = task_query("dnes");
		if(tid != TID_UNDEF)
			return EBADPARM;
		tid = task_create2("dnes", 16000, 0, NesOnMessage, NesOnIdle);
		msg_send_s(tid, 0, EVT_APP_START, 0, pme);
		pme->tid = tid;
		pme->state = NES_STATE_IDLE;
	}
	return SUCCESS;
}

static void	__nesctrl_OnDestroy(t_NESCtrl* pme)
{
	task_end(pme->tid, 0);
	g_object_inherit_destroy(pme);
}


static error_t __nesctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_NESCtrl *pme = (t_NESCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			if(pme->state == NES_STATE_PAUSE){
				t_HGDI gdi = wgt_get_gdi(handle);
				coord_t x, y;
				gu16 w, h;
				DirectDraw_Paint();
				gdi_set_color(gdi, ui_theme_get()->fg_color);
				gdi_set_fontsize(gdi, 1.5);
				w = 80;
				h = 40;
				x = (wgt_bound_dx(handle)-w)/2;
				y = (wgt_bound_dy(handle)-h)/2;
				gdi_draw_text_in(gdi, x, y, w, h, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PAUSE, "Paused"), -1, CHARSET_UTF8, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
				gdi_rectangle(gdi, x, y, w, h, guiNoCorner);
				//gdi_blt(gdi, x, y, w, h);
				g_object_unref(gdi);
			}else if(pme->state == NES_STATE_IDLE){
				//wgt_clear(handle, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
			}
			ret = SUCCESS;
			break;

		case WIDGET_OnSetFocus:
			nesctrl_resume(handle);
			break;
		case WIDGET_OnKillFocus:
			nesctrl_pause(handle);
			break;
		case WIDGET_OnSelected:
			ret = SUCCESS;
			break;
			
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_PRESS && sparam != MMI_KEY_REPEAT)
				break;
			switch(lparam){
				case MVK_UP:
					break;
				case MVK_DOWN:
					break;
				case MVK_LEFT:
					break;
				case MVK_RIGHT:
					break;
				default:
					break;
			}
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove和PenUp
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove和PenUp
					break;
				case MMI_PEN_DOWN:
					break;
				case MMI_PEN_UP:
					break;
				case MMI_PEN_MOVE_IN:
					break;
				case MMI_PEN_MOVE_OUT:
					break;
				case MMI_PEN_MOVE:
					break;
			}
			break;
		}
#endif
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) nesctrlVtbl = {
	(PFNONCREATE)__nesctrl_OnCreate
	, (PFNONDESTROY)__nesctrl_OnDestroy

	, __nesctrl_OnEvent
	, NULL
};

CLASS_DECLARE(nesctrl, t_NESCtrl, nesctrlVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return CURRENT_CLASS;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def


#define NES_CommandReq(pme, param, param2)	\
	msg_send((pme)->tid, 0, EVT_USER, (gu32)param, (gu32)param2, MSG_FLAG_SYNC)

gboolean	nesctrl_checkfile(const char *fname)
{
	return (ROM_IsRomFile(fname)==0)?TRUE:FALSE;
}

gboolean	nesctrl_run( t_HWidget handle, const char *fname, gboolean check)
{
	if(g_object_valid(handle)){
		t_NESCtrl *pme = (t_NESCtrl *)handle;
		if(pme->state == NES_STATE_IDLE && (!check || (ROM_IsRomFile( fname )) == 0) ) {
			t_NESCmdParam param;
			pme->m_nPauseCount = 0;
			param.cmd = NESCMD_START;
			param.fname = fname;
			param.result = 0;
			NES_CommandReq(pme, NESCMD_START, &param);
			if(param.result==0){
				pme->m_nPauseCount = 0;
				pme->state = NES_STATE_RUNNING;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void	nesctrl_stop(t_HWidget handle)
{
	if(g_object_valid(handle)){
		t_NESCtrl *pme = (t_NESCtrl *)handle;
		if(pme->state != NES_STATE_IDLE){
			NES_CommandReq(pme, NESCMD_STOP, NULL);
			pme->state = NES_STATE_IDLE;
			pme->m_nPauseCount = 0;
			wgt_redraw(handle, FALSE);
		}
	}
}

void	nesctrl_pause(t_HWidget handle)
{
	if(g_object_valid(handle)){
		t_NESCtrl *pme = (t_NESCtrl *)handle;
		if(pme->state == NES_STATE_RUNNING){
			NES_CommandReq(pme, NESCMD_PAUSE, NULL);
			pme->state = NES_STATE_PAUSE;
			wgt_redraw(handle, FALSE);
		}
		if(pme->state == NES_STATE_PAUSE){
			pme->m_nPauseCount++;
		}
	}
}

void	nesctrl_resume(t_HWidget handle)
{
	if(g_object_valid(handle)){
		t_NESCtrl *pme = (t_NESCtrl *)handle;
		if(pme->state == NES_STATE_PAUSE){
			if(--pme->m_nPauseCount <= 0){
				pme->m_nPauseCount = 0;
				NES_CommandReq(pme, NESCMD_RESUME, NULL);
				pme->state = NES_STATE_RUNNING;
			}
		}
	}
}

int		nesctrl_state(t_HWidget handle)
{
	return (g_object_valid(handle))?((t_NESCtrl *)handle)->state:NES_STATE_IDLE;
}

#undef	PARENT_CLASS
#undef	CURRENT_CLASS

