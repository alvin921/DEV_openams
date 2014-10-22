#if defined(FTR_TOUCH_SCREEN)
#include "pack_sys.hi"

//1 ======================【TouchScreen Caliberation】======================================
typedef struct {
	t_TsdInterface *vtbl;
	t_TsdCalibPoints points;
	t_HGDI gdi;
}t_TSCalibData;

#define	TS_CALIB_FILE		"a:/.ts_calib"

static const t_TsdCalibPoints gs_calib_const = {
	3,
	{20	, LCD_WIDTH-20	, LCD_WIDTH/2	, 0, 0},
	{20	, LCD_HEIGHT-20	, LCD_HEIGHT/2	, 0, 0},
};

static error_t 	tscalib_WndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_TSCalibData *glob=(t_TSCalibData*)app_get_data();
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			//wgt_enable_attr(handle, WND_ATTR_FULLSCREEN);
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			{
				coord_t x = gs_calib_const.x[glob->points.n];
				coord_t y = gs_calib_const.y[glob->points.n];
				
				gdi_clear(glob->gdi, 0, 0, -1, -1, guiNoCorner);
				gdi_set_color(glob->gdi, RGB_RED);
				gdi_line(glob->gdi, x-5, y, x+5, y);
				gdi_line(glob->gdi, x, y-5, x, y+5);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_END && sparam == MMI_KEY_PRESS){
				task_exit(0);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnPenEvent:
			if(PEN_TYPE(lparam) == MMI_PEN_DOWN){
				PEN_XY(lparam, glob->points.x[glob->points.n], glob->points.y[glob->points.n]);
				glob->points.n++;
				if(glob->points.n == gs_calib_const.n-1){
					if(!glob->vtbl->StartCalib(&gs_calib_const, &glob->points)){
						//2 校准失败，重新校准
						glob->points.n = 0;
					}
					//2 校准成功，等待用户输入最后一点来做验证
				}else if(glob->points.n == gs_calib_const.n){
					//2 校准成功后，还须用最后一点来做验证
					int i = glob->points.n-1;
					if(ABS(glob->points.x[i] - gs_calib_const.x[i]) <= 5 && ABS(glob->points.y[i] - gs_calib_const.y[i]) <= 5){
						//2 校准成功且最后一点验证ok, 保存校准数据
						int fd = fsi_open(TS_CALIB_FILE, _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
						if(fd >= 0){
							fsi_write(fd, &glob->points, sizeof(glob->points));
							fsi_close(fd);
						}
						task_exit(0);
						break;
					}else{
						//2 最后一点验证失败，重新校准
						glob->points.n = 0;
					}
				}
				wnd_redraw(handle, NULL);
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}


//2 <sparam>=0, check & start calib if not calibrated
//2 <sparam>=1, force to start calib
int 		TSCalibAppMsgHandler(t_sparam sparam, t_lparam lparam)
{
	t_TSCalibData *glob;
	int ret = EBADPARM;
	
	t_TsdInterface *vtbl;
	char *model = NULL;
	gboolean force = *((gboolean*)lparam);
	
	vtbl = ams_query_interface("tsd");
	if(vtbl && vtbl->GetModel)
		model = vtbl->GetModel();
	if(vtbl == NULL || model == 0 || *model == 0 || vtbl->StartCalib == NULL){
		return SUCCESS;
	}
	
	glob = MALLOC(sizeof(t_TSCalibData));
	memset(glob, 0, sizeof(t_TSCalibData));
	app_set_data(glob);
	
	glob->vtbl = vtbl;
	if(vtbl->Init)vtbl->Init();
	if(vtbl->Wakeup)vtbl->Wakeup();
	
	if(force == FALSE){
		int fd = fsi_open(TS_CALIB_FILE, _O_RDONLY, _S_IREAD);
		if(fd < 0){
			goto start_calib;
		}
		if(fsi_read(fd, &glob->points, sizeof(glob->points)) != sizeof(glob->points)){
			fsi_close(fd);
			goto start_calib;
		}
		fsi_close(fd);
		if(vtbl->StartCalib){
			glob->points.n--;
			if(vtbl->StartCalib(&gs_calib_const, &glob->points) == FALSE){
				goto start_calib;
			}
		}
		ret = SUCCESS;
		goto out;
	}
start_calib:
	amos_set_tscalib(TRUE);
	glob->points.n = 0;
	glob->gdi = lcd_create_gdi();
	if(wnd_show(wnd_create(tscalib_WndMsgHandler, FALSE, 0)))
		ret = app_loop(NULL);
out:	
	amos_set_tscalib(FALSE);
	if(glob){
		g_object_unref(glob->gdi);
		FREE(glob);
	}
	return ret;
}
#endif

