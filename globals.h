#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "constants.h"
#include "square.h"
#include "selector.h"
#include "vagmanager.h"
#include <stdlib.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

/*
==============================================
|			Global Variables				 |
==============================================
*/

GsOT myOT[2];
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH];
PACKET GPUPacketArena[2][PACKETMAX];

u_long	_ramsize	=	0x00200000;
u_long	_stacksize	=	0x00004000;

short CurrentBuffer	=	0;

POLY_F4 f4[SQUARE_NB_X][SQUARE_NB_Y];
POLY_F4 selector_poly;
POLY_F4 background_poly;

SQUARE cells[SQUARE_NB_X][SQUARE_NB_Y];
SQUARE selector_sprite;
SQUARE background;

SELECTOR selector;

unsigned long ot[SQUARE_NB+2];
unsigned int next_second=0;
unsigned int current_frame=0;
unsigned short fps=0;

int isPaused=1;
int stepMode=0;

unsigned short padStartDown=0;
unsigned short padR1Down=0;
unsigned short padXDown=0;
unsigned short padL1Down=0;

unsigned short padRightDown=0;
unsigned short padLeftDown=0;
unsigned short padDownDown=0;
unsigned short padUpDown=0;

unsigned short state[SQUARE_NB_X][SQUARE_NB_Y];
unsigned short temp[SQUARE_NB_X][SQUARE_NB_Y];
unsigned short selector_field[SQUARE_NB_X][SQUARE_NB_Y];


#endif
