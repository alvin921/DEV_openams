#include "pbx_map.h"


#define CHR_FLOOR		(char)' '
#define CHR_TARGET		(char)'.'
#define CHR_F_PORTER	(char)'@'
#define CHR_T_PORTER	(char)'+'
#define CHR_F_BOX		(char)'$'
#define CHR_T_BOX		(char)'*'
#define CHR_WALL		(char)'#'

#define CHR_SEP				(char)':'


static int __pbx_map_load(pbx_map_t *map, const char *str_map)
{
	gu8 row, col;
	char c;

	memset(map, 0, sizeof(pbx_map_t));
	
	map->porter_row = map->porter_col = -1;

	row = 0;
	while(c = *str_map++){
		col = 0;
		while((c != CHR_SEP) && c){
			switch(c){
				case CHR_TARGET:
					map->data[row][col] = PANE_T_FLOOR;
					map->target++;
					break;
				case CHR_F_PORTER:
				case CHR_T_PORTER:
					map->porter_row = row;
					map->porter_col = col;
					
					if(c == CHR_T_PORTER){
						map->data[row][col] = PANE_T_PORTER;
						map->target++;
					}else{
						map->data[row][col] = PANE_F_PORTER;
					}
					break;
				case CHR_F_BOX:
					map->data[row][col] = PANE_F_BOX;
					//map->f_box++;
					break;
				case CHR_T_BOX:
					map->data[row][col] = PANE_T_BOX;
					map->t_box++;
					map->target++;
					break;
				case CHR_WALL:
					map->data[row][col] = PANE_WALL;
					break;
				case CHR_FLOOR:
				default:
					map->data[row][col] = PANE_FLOOR;
					break;
			}
			col++;
			if(col >= MAP_COL_MAX){
				while(c != CHR_SEP)
					c = *str_map ++;
			}else
				c = *str_map++;
		}
		row++;
		if(row >= MAP_ROW_MAX)
			break;
		if(c == 0)
			break;
	}

	return SUCCESS;
}


const static char map46[] = "########:########:##    ##:##.##$ #:# ..$  #:#  #$  #:#  @ ###:########";
const static char map15[] = "########:########:##  . ##:##@# *##:##  $ ##:## #* ##:##    ##:########";
const static char map37[] = "########:###  ###:##    ##:# . .$##:#  $@  #:####*# #:####   #:########";
const static char map06[] = "########:########:## . * #:##  #  #:##@$*  #:###   ##:###   ##:########";
const static char map98[] = "########:#   #  #:# @  $ #:##$##..#:## .#  #:##$    #:##  ####:########";
const static char map19[] = "########:###  . #:## * # #:## .$  #:##  #$##:### @ ##:########:########";
const static char map58[] = "########:##  .@ #:## #.# #:##   $ #:##.$$ ##:##  ####:########:########";
const static char map23[] = "########:#### @##:#  *$ ##:#     ##:## .####:##$ ####:## .####:########";
const static char map35[] = "########:##.###.#:## #  .#:## $$ @#:##  $  #:##  #  #:##  ####:########";
const static char map65[] = "########:#### @##:####   #:#. #$$ #:#     ##:#.  $###:##.  ###:########";
const static char map44[] = "########:# ..####:# $    #:#  #$# #:# @ .$ #:########:########:########";
const static char map13[] = "########:###  .##:# $ # ##:# *$  ##:# .#@ ##:#    ###:#   ####:########";
const static char map49[] = "########:########:#.  @.##:#  $# ##:# # $. #:#   $# #:####   #:########";
const static char map31[] = "########:#. .####:#.#$$ ##:#   @ ##:# $#  ##:##   ###:########:########";
const static char map40[] = "########:#.  ####:# #   ##:# . # ##:# $*$ ##:##@ ####:##  ####:########";
const static char map96[] = "########:########:#.   . #:# # #  #:#@$  $.#:##### $#:#####  #:########";
const static char map66[] = "########:#  #####:#  #####:# .*   #:##$    #:## #$###:##. @###:########";
const static char map09[] = "########:## @ ###:## .   #:#. $.$ #:##$# ###:##   ###:########:########";
const static char map34[] = "########:##   ###:# $# ###:# . @###:# *   ##:## #$ ##:##.  ###:########";
const static char map14[] = "########:########:##  ####:#..$  .#:# #$ $ #:#@  #  #:#####  #:########";
const static char map92[] = "########:##  .@##:##   $.#:####*# #:##     #:#  $  ##:#   ####:########";
const static char map26[] = "########:##@ ####:##  ####:##. ####:# $$. .#:#  $ ###:###  ###:########";
const static char map39[] = "########:########:##.  ###:## # ###:## *$  #:##  $. #:##  @###:########";
const static char map21[] = "########:########:###   ##:### #.##:###  .##:#@ $$ ##:#  .$ ##:########";
const static char map54[] = "########:#   @###:# $# ###:# * $  #:#   ## #:##.  . #:###   ##:########";
const static char map62[] = "########:##   @##:##  #  #:##.  $ #:## $$#.#:####  .#:########:########";
const static char map80[] = "########:########:###. ###:# .  ###:#   $$ #:## . $@#:########:########";
const static char map72[] = "########:##@.  ##:# $$* ##:#  #  ##:#  #  .#:#### # #:####   #:########";
const static char map74[] = "########:#####  #:#####$.#:###  . #:###  #.#:# $  $ #:#   #@ #:########";
const static char map48[] = "########:#  .####:# $.. ##:#  ##$##:##  #  #:##$   @#:##  ####:########";
const static char map59[] = "########:###  ###:###  ###:### .. #:#  $#  #:#  .$$ #:#### @ #:########";
const static char map51[] = "########:#   ####:# # *@##:#  *   #:###$   #:###   .#:########:########";
const static char map95[] = "########:### .  #:# $@#. #:#  $# ##:#  *  ##:##  # ##:###   ##:########";
const static char map12[] = "########:########:########:##  ####:#     ##:#  #$$@#:#  . *.#:########";
const static char map63[] = "########:##@    #:#. #   #:# $$$.##:# .#  ##:#  #####:########:########";
const static char map25[] = "########:#      #:# # ##*#:# #@ $ #:#.$ .  #:#####  #:#####  #:########";
const static char map78[] = "########:##@   ##:###$   #:### .  #:# $ #$##:# .  .##:####  ##:########";
const static char map41[] = "########:#   ####:#  $  ##:##$$ .##:##@ . ##:### # ##:###  .##:########";
const static char map55[] = "########:#   ####:# $$   #:# .#.  #:#  ## ##:#  ##$##:# @  .##:########";
const static char map87[] = "########:########:########:# .  ###:# .# ###:# @$$  #:# $.   #:########";
const static char map85[] = "########:# @.#  #:# .$ . #:#  #$  #:#  $  ##:###  ###:###  ###:########";
const static char map50[] = "########:##   . #:# $  $@#:#.$.####:#  #####:#  #####:#  #####:########";
const static char map32[] = "########:# .  ###:#  #@###:#  $ ###:##$#  ##:#   # ##:#. *  ##:########";
const static char map47[] = "########:########:#### . #:# *@ . #:# $ #  #:# #  $ #:#   ####:########";
const static char map10[] = "########:########:########:###  ###:# .. $.#:#  $$ @#:####   #:########";
const static char map94[] = "########:########:#####@ #:##### .#:# $ $ $#:#   .  #:### .  #:########";
const static char map97[] = "########:#   #  #:# #.$ $#:#   $  #:#####. #:###   @#:###   .#:########";
const static char map69[] = "########:####@ ##:###  ..#:## $#$##:#   $. #:#  #   #:#    ###:########";
const static char map61[] = "########:#   @###:# $$####:# $ .  #:## #.# #:#.   # #:#      #:########";
const static char map67[] = "########:####  ##:#### $##:# @$.  #:# ##   #:#   ## #:#   * .#:########";
const static char map68[] = "########:#### @ #:####   #:## $ $##:## $  ##:#.  # ##:#..   ##:########";
const static char map29[] = "########:########:####. @#:#  .$  #:# #  ###:# $ $ .#:####   #:########";
const static char map56[] = "########:########:#  .# @#:# # $  #:# $.#$ #:## .   #:##  ####:########";
const static char map22[] = "########:########:##     #:##.## .#:##*  $@#:##  #$ #:##  #  #:########";
const static char map53[] = "########:#. #####:# $#####:#  #####:# .$ @ #:# .$ # #:###    #:########";
const static char map77[] = "########:#      #:# #$   #:# $ @#.#:##$#.  #:##    .#:########:########";
const static char map64[] = "########:#  . ###:#    ###:# #$$. #:#.  ## #:#@$ ## #:###    #:########";
const static char map45[] = "########:###  ###:#   $ ##:# #.#@##:# #$ .##:#  .$ ##:##   ###:########";
const static char map17[] = "########:###   ##:# $ # ##:# $.$. #:# ##.  #:#  @ ###:########:########";
const static char map30[] = "########:###   ##:###$.@ #:#  $.  #:# # * ##:#   ####:########:########";
const static char map07[] = "########:##@ ####:## $  ##:### # ##:#.# #  #:#.$  # #:#.   $ #:########";
const static char map03[] = "########:#  ##  #:# .  .$#:# $# $.#:## @ # #:####   #:########:########";
const static char map05[] = "########:#   #. #:#.  $  #:##$##  #:#  ##$##:#  @   #:# .#   #:########";
const static char map20[] = "########:#   #  #:# . $. #:##$##  #:#  ##$##:#  @ . #:#  #   #:########";
const static char map36[] = "########:#   #  #:#   $  #:##$##  #:# .##$##:# .@   #:# .#   #:########";
const static char map90[] = "########:#   #  #:# . $. #:##$##  #:#  ##$##:# .@   #:#  #   #:########";
const static char map08[] = "########:#   #  #:#  .$  #:##$##  #:# .##$##:#  @.  #:#  #   #:########";
const static char map28[] = "########:#   #  #:#   $  #:##*##  #:# .##$##:# .@   #:#  #   #:########";
const static char map27[] = "########:## ## .#:#  $   #:#@$$   #:# #  # #:# #### #:#.    .#:########";
const static char map02[] = "########:#  #   #:#  # * #:#  # .##:## $   #:# $  . #:#@ ##  #:########";
const static char map24[] = "########:#. #####:#.   ###:# #$   #:#. $$# #:##     #:##### @#:########";
const static char map16[] = "########:##@ ####:#   $. #:#  #   #:#   #* #:## $   #:#### . #:########";
const static char map73[] = "########:#...#  #:# $  $ #:# $#   #:#  #  ##:#  # @##:########:########";
const static char map71[] = "########:#  ... #:# #  # #:# ## # #:# #  # #:#  $$$ #:#    #@#:########";
const static char map01[] = "########:##   ###:# #.$ ##:# .+  ##:# $ $ ##:##   ###:########:########";
const static char map43[] = "########:## ## .#:#    $ #:#@  $$.#:#.#  # #:# #### #:#      #:########";
const static char map60[] = "########:## ## .#:# $ $$ #:#@     #:# #  # #:# #### #:#.    .#:########";
const static char map70[] = "########:#      #:# $$ #.#:# #    #:##  # .#:## # $.#:#@     #:########";
const static char map11[] = "########:#  @   #:# ###.##:#   $.##:## $#. #:##$ #  #:#      #:########";
const static char map75[] = "########:####  ##:## $  ##:# $ #  #:#@$ #  #:## #. .#:##.    #:########";
const static char map76[] = "########:##   ###:##     #:## #.$.#:# . #  #:# $ $ ##:####  @#:########";
const static char map04[] = "########:## @   #:# ## # #:# ## $ #:#   *. #:# #$.# #:#     ##:########";
const static char map38[] = "########:# #  # #:#      #:##. .$##:#  $@  #:#  #*# #:# #    #:########";
const static char map42[] = "########:###    #:###.   #:#  $## #:#. $   #:#  # $##:#   .@##:########";
const static char map88[] = "########:#     ##:#@#    #:### .$ #:# $$# ##:# .  .##:####  ##:########";
const static char map79[] = "########:##    ##:##    ##:##.##$ #:# *.   #:#  # $ #:# @  ###:########";
const static char map93[] = "########:## ..@##:##   $.#:####$# #:##     #:# $   ##:#   ####:########";
const static char map33[] = "########:#  #####:#  .#@ #:# # $  #:#  .#$ #:## . $ #:##  ####:########";
const static char map52[] = "########:#  #   #:# $#   #:#. ##  #:# .$ @ #:# .$ # #:###    #:########";
const static char map18[] = "########:###  . #:## * # #:## .$  #:##  #$##:### @  #:####   #:########";
const static char map86[] = "########:# .###.#:#  #  .#:#  $$ @#:##  $ ##:##  ####:##  ####:########";
const static char map84[] = "########:# @  ###:#  # $ #:# *.   #:##.##$ #:#   # ##:#      #:########";
const static char map82[] = "########:#.   ###:#  $  ##:###*# ##:## @$ ##:##  . ##:####  ##:########";
const static char map83[] = "########:#  ##  #:# .    #:#.## # #:# @$*  #:##$   ##:##   ###:########";
const static char map57[] = "########:@  #####: #$ .  #:    .# #:# $#.$ #:##   ###:########:########";
const static char map81[] = "########:#####  #:#### . #:### $  #: $ $.@##:  .  ###:########:########";
const static char map91[] = "########:# @...##:#   ####:##$    #:   #$# #: $ #   #:   #####:########";
const static char map99[] = "########:####  ##:#  $  ##:# #$#  #:#  $   #:#.###  #:#.#### #:#. @   #";
const static char map89[] = " ##### : #   # : # #$##:## @  #:# .#$ #:# .  ##:# .#$# :##   # ";


const static char * const map_list[] = {
	map01,
	map02,
	map03,
	map04,
	map05,
	map06,
	map07,
	map08,
	map09,
	map10,
	map11,
	map12,
	map13,
	map14,
	map15,
	map16,
	map17,
	map18,
	map19,
	map20,
	map21,
	map22,
	map23,
	map24,
	map25,
	map26,
	map27,
	map28,
	map29,
	map30,
	map31,
	map32,
	map33,
	map34,
	map35,
	map36,
	map37,
	map38,
	map39,
	map40,
	map41,
	map42,
	map43,
	map44,
	map45,
	map46,
	map47,
	map48,
	map49,
	map50,
	map51,
	map52,
	map53,
	map54,
	map55,
	map56,
	map57,
	map58,
	map59,
	map60,
	map61,
	map62,
	map63,
	map64,
	map65,
	map66,
	map67,
	map68,
	map69,
	map70,
	map71,
	map72,
	map73,
	map74,
	map75,
	map76,
	map77,
	map78,
	map79,
	map80,
	map81,
	map82,
	map83,
	map84,
	map85,
	map86,
	map87,
	map88,
	map89,
	map90,
	map91,
	map92,
	map93,
	map94,
	map95,
	map96,
	map97,
	map98,
	map99
};

int map_load(pbx_map_t *map, int stage)
{
	return (map && stage >= 0 && stage < MAP_MAX)? __pbx_map_load(map, map_list[stage]) : EBADPARM;
}

gboolean map_next_cell(pbx_map_t *map, t_MapIndex row, t_MapIndex col, t_MapIndex *next_row, t_MapIndex *next_col, gu8 direction)
{
	switch(direction){
		case guiTop:		row--; 	break;
		case guiRight:	col++;	break;
		case guiBottom:	row++;	break;
		case guiLeft:		col--;	break;
		default: return FALSE;
	}
	if(next_row)
		*next_row = row;
	if(next_col)
		*next_col = col;
	return ((row < 0) || (row >= MAP_ROW_MAX) || (col < 0) || (col >= MAP_COL_MAX))?FALSE:TRUE;
}

