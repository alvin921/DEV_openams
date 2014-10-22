#include "pack_pushbox.h"
#if defined(FTR_SOFT_KEY)
#include "wgt_softkey.h"
#endif
#include "pushbox_resid.h"
#include "pbx_if.h"

static void	CmdOnPrevious(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	pbx_map_change(pbx, -1);
}

static void	CmdOnNext(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	pbx_map_change(pbx, 1);
}

static void	CmdOnUndo(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	pbx_map_undo(pbx);
}

static void	CmdOnRedo(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	pbx_map_redo(pbx);
}

static void	CmdOnReplay(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	if(pbx->push || pbx->step)
	if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, brd_get_string(brd_get(PACK_GUID_pushbox), RESID_STR_PROMPT_REPLAY, "")) == MBB_YES)
		pbx_map_play(pbx);
}

static void	CmdOnDemo(t_HWidget handle, void *userdata)
{
	pbx_t *pbx = userdata;
	pbx_map_demo(pbx);
}
static t_MenuItem miOperation[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_PREVIOUS_STAGE, CmdOnPrevious),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_NEXT_STAGE, CmdOnNext),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_UNDO, CmdOnUndo),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REDO, CmdOnRedo),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REPLAY, CmdOnReplay),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DEMO, CmdOnDemo),
	MITEM_END
};

static error_t __on_win_key(t_HWindow handle, pbx_t *pbx, amos_key_t key)
{
	error_t handled = NOT_HANDLED;
	gboolean moved = FALSE;

	switch(key){
		case MVK_UP:
			moved = pbx_map_move(pbx, guiTop, ACT_NORMAL);
			handled = SUCCESS;
			break;
		case MVK_DOWN:
			moved = pbx_map_move(pbx, guiBottom, ACT_NORMAL);
			handled = SUCCESS;
			break;
		case MVK_LEFT:
			moved = pbx_map_move(pbx, guiLeft, ACT_NORMAL);
			handled = SUCCESS;
			break;
		case MVK_RIGHT:
			moved = pbx_map_move(pbx, guiRight, ACT_NORMAL);
			handled = SUCCESS;
			break;
		case MVK_2: //2 prev 5 stages
			pbx_map_change(pbx, -5);
			handled = SUCCESS;
			break;
		case MVK_8: //2 next 5 stages
			pbx_map_change(pbx, 5);
			handled = SUCCESS;
			break;
		case MVK_4: //2 prev 1 stage
			pbx_map_change(pbx, -1);
			handled = SUCCESS;
			break;
		case MVK_6: //2 next 1 stage
			pbx_map_change(pbx, 1);
			handled = SUCCESS;
			break;
		case MVK_1: //2 UNDO
			pbx_map_undo(pbx);
			handled = SUCCESS;
			break;
		case MVK_3: //2 REDO
			pbx_map_redo(pbx);
			handled = SUCCESS;
			break;
		case MVK_STAR:	//2 answer demo
			pbx_map_demo(pbx);
			handled = SUCCESS;
			break;
		case MVK_HASH: //2 replay
			if(pbx->push || pbx->step)
			if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, brd_get_string(brd_get(PACK_GUID_pushbox), RESID_STR_PROMPT_REPLAY, "")) == MBB_YES)
				pbx_map_play(pbx);
			handled = SUCCESS;
			break;
		case MVK_RSK:
		case MVK_CLR:
		case MVK_BACK:
			if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, brd_get_string(brd_get(PACK_GUID_pushbox), RESID_STR_PROMPT_QUIT, "")) == MBB_YES)
				wnd_send_close(handle, 0);
			handled = SUCCESS;
			break;
	}
	if(moved){
		if(pbx_map_success(pbx)){
			msgbox_show(NULL, _CSK(OK), 0, 0, brd_get_string(brd_get(PACK_GUID_pushbox), RESID_STR_PROMPT_DONE, ""));
		}
	}
	return handled;
}
error_t	pushbox_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	pbx_t *pbx = (pbx_t *)wgt_get_tag(handle);
	error_t handled = NOT_HANDLED;
#if defined(FTR_TOUCH_SCREEN)
	coord_t x, y;
#endif

	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			pbx = pbx_init(handle, 0, 0, lcd_get_width(), lcd_get_height());
			if(pbx == NULL)
				return ENO_MEMORY;
			wgt_set_tag(handle, pbx);
			{
				t_HWidget sk = wnd_get_skbar(handle);
				t_HWidget menu;
				menu = wnd_get_widget(handle, RESID_MENU_OPTION);
				menu_bind(menu, miOperation, pbx);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_popup(sk, SK_LSK, menu);
				}
			}
			handled = SUCCESS;
			break;
		case WINDOW_OnClose:
			pbx_free(pbx);
			handled = SUCCESS;
			break;
		case WINDOW_OnDraw:
			pbx_map_show(pbx, TRUE);
			handled = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_REPEAT || sparam == MMI_KEY_PRESS)
				handled = __on_win_key(handle, pbx, (amos_key_t)lparam);
			break;
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			PEN_XY(lparam, x, y);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_DOWN:
					if(wgt_point_in_client((t_HWidget)handle, &x, &y) && x >= pbx->x && x < pbx->x+pbx->dx && y>=pbx->y && y<pbx->y+pbx->dy){
						int row, col;
						col = (x - pbx->x)/pbx->pane_width;
						row = (y - pbx->y)/pbx->pane_height;
						if(row>=0&&row<MAP_ROW_MAX&&col>=0&&col<MAP_COL_MAX){
							pbx->pen_down = TRUE;
						}
					}
					handled = SUCCESS;
					break;
				case MMI_PEN_UP:
					if(pbx->pen_down){
						if(wgt_point_in_client((t_HWidget)handle, &x, &y) && x >= pbx->x && x < pbx->x+pbx->dx && y>=pbx->y && y<pbx->y+pbx->dy){
							int row, col;
							col = (x - pbx->x)/pbx->pane_width;
							row = (y - pbx->y)/pbx->pane_height;
							if(row>=0&&row<MAP_ROW_MAX&&col>=0&&col<MAP_COL_MAX){
								if(pbx_map_move_to(pbx, row, col, ACT_NORMAL)){
									if(pbx_map_success(pbx)){
										msgbox_show(NULL, _CSK(OK), 0, 0, brd_get_string(brd_get(PACK_GUID_pushbox), RESID_STR_PROMPT_DONE, ""));
									}
								}
							}
						}
					}
					pbx->pen_down = FALSE;
					handled = SUCCESS;
					break;
			}
			break;
#endif
	}

	return handled;
}

