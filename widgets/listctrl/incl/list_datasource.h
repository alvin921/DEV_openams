#ifndef LIST_DATASOURCE_H
#define LIST_DATASOURCE_H

#include "widget.h"

typedef enum {
	IDENT_BY_POSITION = BIT_0
	, IDENT_BY_ID = BIT_1
}t_IdentType;


enum {
	GUI_VIEWTYPE_NORMAL = 0x00,

		GUI_VIEWTYPE_LIST_USER = 0x10,
		
		GUI_VIEWTYPE_CUSTOM = 0x7f,				//2 �������ͣ���datasource�ṩMeasure�ӿڷ���ÿ��item(width, height)

	GUI_VIEWTYPE_ICON = 0x80,		//2 ��iconΪ��
	
		GUI_VIEWTYPE_ICON_TEXT_BOTTOM = GUI_VIEWTYPE_ICON,	//2 text��icon���棬��iconΪ��
		GUI_VIEWTYPE_ICON_TEXT_TOP, 	//2 text��icon���棬��iconΪ��
		GUI_VIEWTYPE_ICON_TEXT_TITLE,	//2 text����cell����ʾ�����Ǹ��µ�title
		GUI_VIEWTYPE_ICON_ONLY,			//2 ���ı�����������ͼ

		GUI_VIEWTYPE_ICON_USER = 0x90
};

typedef gu8 t_GuiViewType;
#define	VIEWTYPE_IS_NORMAL(viewtype)		((viewtype) >= 0 && (viewtype) < GUI_VIEWTYPE_CUSTOM)
#define	VIEWTYPE_IS_CUSTOM(viewtype)		((viewtype) == GUI_VIEWTYPE_CUSTOM)
#define	VIEWTYPE_IS_ICON(viewtype)		((viewtype)&GUI_VIEWTYPE_ICON)



typedef t_HWidget	t_HList;

//====================================================================
//2 DataSource definition
typedef struct {
	t_GObject	common;
	t_HList		list;
}t_DataSource;

typedef t_DataSource * t_HDataSource;

#define DS_LISTCTRL(ds)	(((t_DataSource*)(ds))->list)

VTBL_DECLARE(datasource)
{
	INHERIT_BASE_VTBL(datasource);

	//2 ����ֵ��BOOL��Ϊgboolean������LISTSOURCE_CanFocused���жϷ���ֵ���쳣
	gboolean	(*Measure)(t_HDataSource h, t_GuiViewType, int index, int *width, int *height);
	void		(*Redraw)(t_HDataSource h, t_HGDI gdi, t_GuiViewType, int index, int width, int height, BOOL hilight);
	int		(*GetCount)(t_HDataSource h);
	gboolean	(*CanFocused)(t_HDataSource h, int index);
	error_t	(*HookEvent)(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);
	void		(*Refresh)(t_HDataSource h); // return count
	#if 0 //2 ����ListCtrl��˵, datasource�ṩDelete&DeletaAll������û������, ��Ϊlistctrl��������ȥɾ������, ֻ�п����߲�֪��ʱɾ��
	void	(*Delete)(void*, int index);
	void	(*DeleteAll)(void*);
	#endif
};

#define	LISTSOURCE_Measure(ds, type, index, width, height)	(OBJECT_INTERFACE(ds, VTBL(datasource), Measure)?OBJECT_INTERFACE(ds, VTBL(datasource), Measure)(ds, type, index, width, height) : FALSE)
#define	LISTSOURCE_Redraw(ds, cr, type, index, width, height, hilight)	do{if(OBJECT_INTERFACE(ds, VTBL(datasource), Redraw)) \
		OBJECT_INTERFACE(ds, VTBL(datasource), Redraw)(ds, cr, type, index, width, height, hilight);}while(0)
#define	LISTSOURCE_GetCount(ds)	(OBJECT_INTERFACE(ds, VTBL(datasource), GetCount)? OBJECT_INTERFACE(ds, VTBL(datasource), GetCount)(ds) : 0)
#define	LISTSOURCE_CanFocused(ds, index)	(OBJECT_INTERFACE(ds, VTBL(datasource), CanFocused)? OBJECT_INTERFACE(ds, VTBL(datasource), CanFocused)(ds, index) : TRUE)
#define	LISTSOURCE_HookEvent(ds, e, s, l)	(OBJECT_INTERFACE(ds, VTBL(datasource), HookEvent)? OBJECT_INTERFACE(ds, VTBL(datasource), HookEvent)(ds, e, s, l) : NOT_HANDLED)
#define	LISTSOURCE_Refresh(ds)	do{if(OBJECT_INTERFACE(ds, VTBL(datasource), Refresh)) OBJECT_INTERFACE(ds, VTBL(datasource), Refresh)(ds);}while(0)
//====================================================================


#endif	/* LIST_DATASOURCE_H */

