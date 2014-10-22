#ifndef PBX_IF_H
#define PBX_IF_H

#include "pack_pushbox.h"

#include "pushbox_resid.h"

#include "pbx_map.h"


enum {
	PBX_SUCCESS = SUCCESS
		
	, PBX_E_0 = ERROR_SOFT_USER
	, PBX_E_CANNOT_MOVE
};

enum {
	ACT_NORMAL
	, ACT_UNDO
	, ACT_REDO
	, ACT_N
};

#define ACT_MOVE		0
#define ACT_PUSH		1

typedef struct {
	gu8			type;
	gu8			prow;
	gu8			pcol;
}pbx_action_t;

#define ACT_HST_MAX		200

typedef struct {
	/* gui */
	t_HWindow	window;
	t_HGDI	gdi;
	gui_rect_t	canvas;

	t_HImage pane_images;
	gu16	pane_width;
	gu16	pane_height;

	gboolean	pen_down;
	MC_FILL_STRUCT3;

	coord_t	x;
	coord_t	y;
	gu16	dx;
	gu16	dy;

	/* map */
	gs16	stage_cur;
	gs16	stage_num;
	pbx_map_t	map;

	// action
	gu16				step;
	gu16				push;
	// history
	pbx_action_t		history[ACT_HST_MAX];
	gs16				hst_front;		// front of queue
	gs16				hst_tail;		// tail of queue
	gs16				hst_cur;
}pbx_t;


BEGIN_DECLARATION

pbx_t * 	pbx_init(t_HWindow h, coord_t x, coord_t y, gu16 dx, gu16 dy);
void		pbx_free(pbx_t *pbx);
gboolean		pbx_map_move(pbx_t *pbx, gu8 direction, gu8 mode);
gboolean		pbx_map_move_to(pbx_t *pbx, t_MapIndex row, t_MapIndex col, gu8 mode);
gboolean		pbx_map_undo(pbx_t *pbx);
gboolean		pbx_map_redo(pbx_t *pbx);

gboolean	pbx_map_success(pbx_t *pbx);
void		pbx_map_show(pbx_t *pbx, gboolean defer);
void		pbx_map_play(pbx_t *pbx);
void		pbx_map_change(pbx_t *pbx, int num);
void pbx_map_demo(pbx_t *pbx);

END_DECLARATION

#endif /* PBX_IF_H */
