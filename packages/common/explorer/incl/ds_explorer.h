#ifndef DS_EXPLORER_H
#define DS_EXPLORER_H

//2 !!!说明!!!
/*
本控件只提供基本的目录/文件浏览服务, 以及由此为基础的FileDialog
不提供诸如文件copy/rename/unlink/open等操作, 这些都是explorer应用需要做的
*/

#include "list_datasource.h"

#include "ams.h"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_LISTCTRL)
__declspec(dllimport) extern t_GClass	 class_DSExplorer;
#else
extern t_GClass class_DSExplorer;
#endif

enum {
	DENTRY_VIEWTYPE_LIST = GUI_VIEWTYPE_NORMAL,
	DENTRY_VIEWTYPE_ICON = GUI_VIEWTYPE_ICON
};

#define	DENTRY_FLAG_FILE		BIT_0
#define	DENTRY_FLAG_FOLDER		BIT_1

#define	DENTRY_FLAG_DEFAULT		(DENTRY_FLAG_FILE | DENTRY_FLAG_FOLDER)

#define	DENTRY_FLAG_UNCHANGED		0xFF


typedef void	(*f_DentrySelect)(t_HWidget handle, const char *path, const char *name);
typedef void	(*f_DentryChange)(t_HWidget handle, const char *path);


BEGIN_DECLARATION

t_HDataSource	DSExplorer_Create(void);
void		DSExplorer_Bind(t_HDataSource ds, f_DentrySelect select, f_DentryChange change);
int 	DSExplorer_SetPath(t_HDataSource handle, const char *path);
int	DSExplorer_SetFilter(t_HDataSource handle, const char *filter, gu32 flag);
const char *DSExplorer_GetPath(t_HDataSource handle);
const char *DSExplorer_GetFname(t_HDataSource handle);

void	listctrl_attach_explorer(t_HWidget handle, const char *path, const char *filter, gu32 flag, f_DentrySelect select, f_DentryChange change);
t_HImage	sys_get_file_icon(gu8 mode, const char *fname);

END_DECLARATION

#endif	/* DS_EXPLORER_H */

