
#ifndef PBX_MAP_H
#define PBX_MAP_H

#include "ams.h"



#define MAP_MAX				99

enum {
	PANE_WALL = 0
		
	, PANE_FLOOR
	, PANE_F_FLOOR = PANE_FLOOR
	, PANE_T_FLOOR		// target floor
	
	, PANE_BOX
	, PANE_F_BOX = PANE_BOX		// box on the floor
	, PANE_T_BOX		// box on the target place

	, PANE_PORTER
	, PANE_F_PORTER = PANE_PORTER // porter on the floor
	, PANE_T_PORTER	// porter on the target floor

	, PANE_N

	, PANE_EDGE = 0xFF
};

#define PANE_IS_PORTER(role)	( (role) == PANE_F_PORTER || (role) == PANE_T_PORTER )
#define PANE_IS_FLOOR(role)		( (role) == PANE_FLOOR || (role) == PANE_T_FLOOR )
#define PANE_IS_BOX(role)		( (role) == PANE_F_BOX || (role) == PANE_T_BOX )

#define PANE_MOVABLE(role)	( PANE_IS_FLOOR(role) || PANE_IS_PORTER(role) )
#define	MAP_ROW_MAX		8
#define	MAP_COL_MAX		8

typedef	gs8		t_MapIndex;
//#define MAP_INDEX(row,col)	(((row)<<4) | (col))

typedef struct {
	gu8		data[MAP_ROW_MAX][MAP_COL_MAX];	// map data of that stage
	
	t_MapIndex		porter_row;	// -1 if no porter
	t_MapIndex		porter_col;	// -1 if no porter
	gu8		target;	// number of target places
	gu8		t_box;	// number of boxes on target place
}pbx_map_t;
#define	MAP_ROLE(map, row, col)		((map)->data[row][col])

BEGIN_DECLARATION

int map_load(pbx_map_t *map, int stage);
gboolean map_next_cell(pbx_map_t *map, t_MapIndex row, t_MapIndex col, t_MapIndex *next_row, t_MapIndex *next_col, gu8 direction);


END_DECLARATION

#endif
