#include "pack_explorer.hi"
#include "wgt_listctrl.h"
#include "wgt_menu.h"
#include "ds_explorer.h"

error_t	explorer_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HWidget list, menu;
			gui_rect_t client;
			char *path = (char *)lparam;
			dentry_stat_t st;

			wgt_enable_attr(handle, WND_ATTR_HAS_STATUS|WND_ATTR_HAS_TITLE|WND_ATTR_AUTO_CLOSE);
			wgt_get_client_rect((t_HWidget)handle, &client);

			list = (t_HWidget)wnd_get_widget(handle, RESID_DENTRY);
			wgt_set_bound((t_HWidget)list, 0, 0, client.dx, client.dy);

			if(path && fsi_is_abspath(path) && fsi_stat(path, &st) == 0){
				//2 提供了文件名，则定位到文件所在目录
				if(st.mode & F_MODE_DIR){
					wgt_explorer_set(list, path, NULL, DENTRY_FLAG_UNCHANGED, NULL);
				}else{
					char *p = path+strlen(path)-1;
					while(p > path && (*p != '/' && *p != '\\')){
						p--;
					}
					if(*p == '/' || *p == '\\'){
						*p = 0;
						wgt_explorer_set(list, path, NULL, DENTRY_FLAG_UNCHANGED, NULL);
					}
					
				}
			}
			ret = SUCCESS;
		}	
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//gdi_draw_text_at(wgt_gdi(handle), "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
	}
	return ret;
}

