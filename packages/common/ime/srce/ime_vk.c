#if defined(FTR_NO_INPUT_KEY)
#include "pack_ime.hi"
#include "ime_agent.h"
#include "ime.h"

#undef PARENT_CLASS
#define PARENT_CLASS	NULL


static t_IMEElement	gaVKIMESupported[] = {
	{LANG_123			,"123"	,"123"}
	, {aLANG_ENGLISH		,"abc"		,"abc"}
//	, {LANG_UNDEF		,NULL		,NULL}
};
static error_t	IME_VK_WndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);

t_IMEInfo		gvIMEVk = {
	"VK",
	IME_USER,
	ARRAY_SIZE(gaVKIMESupported),
	gaVKIMESupported,
	IME_VK_WndMsgHandler,
};

enum {
	R_DISABLED,
	R_UCS,
	R_BKSPACE,
	R_USER,
};
#define	VK_MAP_ROW	4
typedef const struct {
	coord_t	x;
	coord_t	dx;
	gu8		r;
	MC_FILL_STRUCT1;
	ucs2_t	ucs;
}t_VkRegion;
typedef const struct {
	coord_t	y;
	gu8		dy;
	gu8		nr;
	t_VkRegion *pr;
}t_VkRow;

typedef struct {
	resid_t	resid;
	gu8		nrow;
	MC_FILL_STRUCT3;
	t_VkRow	*rows;
}t_VkMap;

//2 ¡¾VK Map for Digit¡¿
enum {
	R_DIGIT_LSK = R_USER,
};
static t_VkRegion	ga_digit_vk_r1[] = {
	{1	, 78	, R_UCS	, '1'},
	{81	, 78	, R_UCS	, '2'},
	{161, 78	, R_UCS	, '3'},
};

static t_VkRegion	ga_digit_vk_r2[] = {
	{1	, 78	, R_UCS	, '4'},
	{81	, 78	, R_UCS	, '5'},
	{161, 78	, R_UCS	, '6'},
};
static t_VkRegion	ga_digit_vk_r3[] = {
	{1	, 78	, R_UCS	, '7'},
	{81	, 78	, R_UCS	, '8'},
	{161, 78	, R_UCS	, '9'},
};
static t_VkRegion	ga_digit_vk_r4[] = {
	{1	, 78	, R_DIGIT_LSK	, 0},
	{81	, 78	, R_UCS	, '0'},
	{161, 78	, R_BKSPACE, 0},
};
static t_VkRow	ga_vk_digit_row[VK_MAP_ROW] = {
	{1	, 34	, ARRAY_SIZE(ga_digit_vk_r1)	, ga_digit_vk_r1},
	{35	, 34	, ARRAY_SIZE(ga_digit_vk_r2)	, ga_digit_vk_r2},
	{69	, 34	, ARRAY_SIZE(ga_digit_vk_r3)	, ga_digit_vk_r3},
	{103, 34	, ARRAY_SIZE(ga_digit_vk_r4)	, ga_digit_vk_r4},
};

//2 ¡¾VK Map for English&Chinese¡¿
enum{
	R_EN_SHIFT = R_USER,
	R_EN_123,
	R_EN_IME,
	R_EN_SPACE,
	R_EN_ENTER,

	EN_R_MAX
};

static t_VkRegion	ga_vk_r1[] = {
	{2	, 20	, R_UCS	, 'q'},
	{26	, 20	, R_UCS	, 'w'},
	{50	, 20	, R_UCS	, 'e'},
	{74	, 20	, R_UCS	, 'r'},
	{98	, 20	, R_UCS	, 't'},
	{122, 20	, R_UCS	, 'y'},
	{146, 20	, R_UCS	, 'u'},
	{170, 20	, R_UCS	, 'i'},
	{194, 20	, R_UCS	, 'o'},
	{218, 20	, R_UCS	, 'p'},
};

static t_VkRegion	ga_vk_r2[] = {
	{14	, 20	, R_UCS	, 'a'},
	{38	, 20	, R_UCS	, 's'},
	{62	, 20	, R_UCS	, 'd'},
	{86	, 20	, R_UCS	, 'f'},
	{110, 20	, R_UCS	, 'g'},
	{134, 20	, R_UCS	, 'h'},
	{158, 20	, R_UCS	, 'j'},
	{182, 20	, R_UCS	, 'k'},
	{206, 20	, R_UCS	, 'l'},
};

static t_VkRegion	ga_vk_r3[] = {
	{2	, 27	, R_EN_SHIFT	, 0},
	{38	, 20	, R_UCS	, 'z'},
	{62	, 20	, R_UCS	, 'x'},
	{86	, 20	, R_UCS	, 'c'},
	{110, 20	, R_UCS	, 'v'},
	{134, 20	, R_UCS	, 'b'},
	{158, 20	, R_UCS	, 'n'},
	{182, 20	, R_UCS	, 'm'},
	{211, 27	, R_BKSPACE, 0},
};

static t_VkRegion	ga_vk_r4[] = {
	{2	, 26	, R_EN_123	, 0},
	{32	, 26	, R_EN_IME	, 0},
	{62	, 116, R_EN_SPACE, 0},
	{182, 56	, R_EN_ENTER, 0},
};
static t_VkRow	ga_en_vk_row[VK_MAP_ROW] = {
	{3	, 30	, ARRAY_SIZE(ga_vk_r1)	, ga_vk_r1},
	{37	, 30	, ARRAY_SIZE(ga_vk_r2)	, ga_vk_r2},
	{72	, 30	, ARRAY_SIZE(ga_vk_r3)	, ga_vk_r3},
	{107, 30	, ARRAY_SIZE(ga_vk_r4)	, ga_vk_r4},
};

enum {
	IME_S_DIGIT_1,
	IME_S_DIGIT_2,
	IME_S_EN,
	IME_S_EN_DIGIT,
	IME_S_EN_SYM,
	IME_S_SC_PY,
	IME_S_SC_PY_DIGIT,
	IME_S_SC_SYM,

	IME_S_MAX,
};

static t_VkMap ga_ime_vk[] = {
	//2 Digit
	{RESID_IMG_IME_DIGIT_1	, ARRAY_SIZE(ga_vk_digit_row)	, ga_vk_digit_row},
	{RESID_IMG_IME_DIGIT_2	, ARRAY_SIZE(ga_vk_digit_row)	, ga_vk_digit_row},
	//2 English
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
	//2 Simple Chinese
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
	{RESID_IMG_IME_VK	, ARRAY_SIZE(ga_en_vk_row)	, ga_en_vk_row},
};
static t_VkRegion*	__vk_get(gu8 state, coord_t x, coord_t y, gui_rect_t *out)
{
	if(state != 0xff){
		int i, j;
		t_VkMap *map = &ga_ime_vk[state];
		for(i = 0; i < map->nrow; i++){
			if(y >= map->rows[i].y && y < map->rows[i].y+map->rows[i].dy){
				for(j = 0; j < map->rows[i].nr; j++){
					if(x >= map->rows[i].pr[j].x && x < map->rows[i].pr[j].x+map->rows[i].pr[j].dx){
						if(out){
							out->x = map->rows[i].pr[j].x;
							out->y = map->rows[i].y;
							out->dx = map->rows[i].pr[j].dx;
							out->dy = map->rows[i].dy;
						}
						return &map->rows[i].pr[j];
					}
				}
			}
		}
	}
	return NULL;
}

typedef struct {
	t_HImage	vk_image;
	t_LangID		lang;		//2 default LANG_123, digital
	gu8			t_digit:2;	//2 DIGIT_TYPE_XXX
	gu8			t_case:2;	//2 IME_CASE_XXX
	gu8			s_idx;	//2 ime type <IME_S_XXX>, used to load <vk_image>
	MC_FILL_STRUCT1;
	gu16		image_width;
	gu16		image_height;
	//gui_rect_t	rc;
	gui_rect_t	prev_rt;
	t_VkRegion	*prev_r;
}t_VkIme;

static void	__ShowRegion(t_HWindow handle, gui_rect_t *prc, gboolean focus)
{
	t_VkIme *wdata = (t_VkIme *)wgt_get_tag(handle);
	t_HGDI gdi = wgt_get_gdi((t_HWidget)handle);
	if(g_object_valid(gdi)){
		if(focus){
			gdi_set_color(gdi, 0xa00000e0);
			gdi_rectangle_fill(gdi, prc->x, prc->y, prc->dx, prc->dy, guiFourCorner);
		}else{
			gdi_draw_image_at(gdi, prc->x, prc->y, wdata->vk_image, prc->x, prc->y, prc->dx, prc->dy, 1.0, 1.0);
		}
		gdi_blt(gdi, prc->x, prc->y, prc->dx, prc->dy);
		g_object_unref(gdi);
	}
}
static void	__ShowIME(t_HWindow handle, gboolean defer)
{
	t_VkIme *wdata = (t_VkIme *)wgt_get_tag(handle);
	t_HGDI gdi = wgt_get_gdi(handle);
	if(g_object_valid(gdi)){
		gdi_draw_image_at(gdi, 0, 0, wdata->vk_image, 0, 0, -1, -1, 1.0, 1.0);
		if(!defer)
			gdi_blt(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
		g_object_unref(gdi);
	}
}
static void	IME_OnConfig(t_HWindow handle, t_VkIme *wdata, t_sparam sparam, t_lparam lparam)
{
		gu8 s = 0xff;
		wdata->lang = sparam;
		switch(wdata->lang){
		case LANG_123:
			if(lparam != (t_lparam)(-1))
				wdata->t_digit = lparam;
			s = IME_S_DIGIT_1;
			break;
		case aLANG_ENGLISH:
			if(lparam != (t_lparam)(-1))
				wdata->t_case = lparam;
			s = IME_S_EN;
			break;
		default:
			break;
		}
		if(s != 0xff && wdata->s_idx != s){
			wdata->s_idx = s;
			g_object_unref_0(wdata->vk_image);
			wdata->vk_image = brd_get_image(brd_get(PACK_GUID_SELF), ga_ime_vk[s].resid);
			wdata->image_width = image_get_width(wdata->vk_image);
			wdata->image_height = image_get_height(wdata->vk_image);
		}
}

static error_t	IME_VK_WndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_VkIme *wdata = (t_VkIme *)wgt_get_tag(handle);
	
	switch(evt){
		case WIDGET_OnMessage:
			{
				t_AmosMsg *msg = (t_AmosMsg *)lparam;
				switch(sparam){
					case EVT_IME_CONFIG:
						IME_OnConfig(handle, wdata, msg->sparam, msg->lparam);
						ret = SUCCESS;
						break;
				}
			}
			break;
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			wdata = MALLOC(sizeof(t_VkIme));
			memset(wdata, 0, sizeof(t_VkIme));
			wgt_set_tag(handle, wdata);
			wdata->lang = LANG_UNDEF;
			wdata->s_idx = 0xff;
			wdata->t_digit = DIGIT_TYPE_NUMBER;
			wdata->t_case = IME_CASE_LOWER;

			IME_OnConfig(handle, wdata, LANG_123, DIGIT_TYPE_NUMBER);
			wgt_set_bound((t_HWidget)handle, 0, lcd_get_height()-wdata->image_height, lcd_get_width(), wdata->image_height);
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				g_object_unref(wdata->vk_image);
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			__ShowIME(handle, TRUE);
			ret = SUCCESS;
			break;
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			{
				coord_t x, y;
				PEN_XY(lparam, x, y);
				{
					t_VkRegion *r;
					gui_rect_t rt;
					ret = SUCCESS;
					r = __vk_get(wdata->s_idx, x, y, &rt);
					switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						wdata->prev_r = r;
						if(r != NULL){
							memcpy(&wdata->prev_rt, &rt, sizeof(gui_rect_t));
							__ShowRegion(handle, &rt, TRUE);
						}
						break;
					case MMI_PEN_MOVE:
						if(r != NULL && wdata->prev_r != r){
							if(wdata->prev_r != NULL)
								__ShowRegion(handle, &wdata->prev_rt, FALSE);
							wdata->prev_r = r;
							memcpy(&wdata->prev_rt, &rt, sizeof(gui_rect_t));
							__ShowRegion(handle, &rt, TRUE);
						}
						break;
					case MMI_PEN_UP:
						if(wdata->prev_r != NULL){
							__ShowRegion(handle, &wdata->prev_rt, FALSE);
							wdata->prev_r = -1;
						}
						if(r != NULL && r->r != R_DISABLED){
							 if(r->r == R_UCS){
								ime_agent_add_char(IME_ADD_MODE_NORMAL, r->ucs);
							}else if(r->r ==  R_BKSPACE){
								ime_agent_add_char(IME_ADD_MODE_BKSPACE, 0);
							}else if(wdata->lang == LANG_123){
								if(r->r == R_DIGIT_LSK){
									if(wdata->t_digit == DIGIT_TYPE_PHONE){
										if(wdata->s_idx == IME_S_DIGIT_1){
											wdata->s_idx = IME_S_DIGIT_2;
										}else{
											wdata->s_idx = IME_S_DIGIT_1;
										}
										g_object_unref_0(wdata->vk_image);
										wdata->vk_image = brd_get_image(brd_get(PACK_GUID_SELF), ga_ime_vk[wdata->s_idx].resid);
										wdata->image_width = image_get_width(wdata->vk_image);
										wdata->image_height = image_get_height(wdata->vk_image);
										__ShowIME(handle, FALSE);
									}else if(wdata->t_digit == DIGIT_TYPE_FLOAT){
										ime_agent_add_char(IME_ADD_MODE_NORMAL, '.');
									}
								}
							}else if(r->r ==  R_EN_SPACE){
								ime_agent_add_char(IME_ADD_MODE_NORMAL, ' ');
							}else if(r->r == R_EN_123){
								//2 ÇÐ»»µ½Êý×Ö¼üÅÌ
							}else if(r->r == R_EN_SHIFT){
								//2 ÇÐ»»´óÐ¡Ð´
							}else if(r->r ==  R_EN_IME){
								//2 ÇÐ»»ÊäÈë·¨
							}else if(r->r ==  R_EN_ENTER){
								//2 »»ÐÐ
							}
						}
						break;
					}
				}
			}
			break;
#endif
	}
	return ret;
}

#undef PARENT_CLASS

#endif
