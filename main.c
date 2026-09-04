#include "globals.h"
#include <stdio.h>

/*
==============================================
|			Functions Prototypes			 |
============================================== 
*/

// Initialize graphics settings
void graphics();

// Display what has been drawn in the framebuffer
void display();

// Read the status of the controller on port 1
void pad_read();

// Manage the status of the selector (whether live or dead cell selected)
void selector_status(SELECTOR* selector);

// Check the live status of the 8 neighbor (in french : voisin) cells of a given cell
unsigned short voisin(unsigned short x, unsigned short y);

// Change status of cells in a temporary field
void calc();

// Draw all prims in framebuffer
void draw();

// Manage direction of selector in an unstepped way
void direction(int padd);

// Manage direction of selector in step mode
void direction_step(int padd);

// Initialize basically everything
void init();

/*
==============================================
|				MAIN PROGRAM				 |
==============================================
*/

int main(){
	init();
	PlaySFX();
	while(1){
		current_frame++;
		pad_read();
		if(isPaused==0){
			calc();
		}
		FntPrint("fps : %d\n", fps);
		draw();
		display();
	}
	PadStop();
	return 0;	
}

/*
==============================================
|			Functions Definitions			 |
==============================================
*/

void graphics(){
	unsigned short i;
	unsigned short j;
	unsigned int k = 0;
	SetVideoMode(MODE_NTSC);
	
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 0, 0);
	GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	
	myOT[0].length	=	OT_LENGTH;
	myOT[1].length	=	OT_LENGTH;
	myOT[0].org		=	myOT_TAG[0];
	myOT[1].org		=	myOT_TAG[1];
	
	GsClearOt(0,0,&myOT[0]);
	GsClearOt(0,0,&myOT[1]);
	
	ClearOTagR(ot, SQUARE_NB+2);
	SetPolyF4(&selector_poly);
	addPrim(&ot[0], &selector_poly);
	k++;

	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0; j<SQUARE_NB_Y; j++){
			SetPolyF4(&f4[i][j]);
			addPrim(&ot[k], &f4[i][j]);
			k++;
		}
	}
	SetPolyF4(&background_poly);
	addPrim(&ot[SQUARE_NB+1], &background_poly);
}

void display() {
	unsigned short i;
	unsigned short j;
	FntFlush(-1);
	
	setRGB0(&selector_poly, selector.sprite.red, selector.sprite.green, selector.sprite.blue);
	setXY4(&selector_poly, selector.sprite.x0, selector.sprite.y0, selector.sprite.x1, selector.sprite.y1, selector.sprite.x2, selector.sprite.y2, selector.sprite.x3, selector.sprite.y3);
	
	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0; j<SQUARE_NB_Y; j++){
			setRGB0(&f4[i][j],cells[i][j].red,cells[i][j].green,cells[i][j].blue);
			setXY4(&f4[i][j],cells[i][j].x0,cells[i][j].y0,cells[i][j].x1,cells[i][j].y1,cells[i][j].x2,cells[i][j].y2,cells[i][j].x3,cells[i][j].y3);
		}	
	}
	
	setRGB0(&background_poly, background.red, background.green, background.blue);
	setXY4(&background_poly, background.x0, background.y0, background.x1, background.y1, background.x2, background.y2, background.x3, background.y3);
	
	DrawOTag(ot+SQUARE_NB+1);
	
	CurrentBuffer	=	GsGetActiveBuff();
	
	GsSetWorkBase((PACKET*)GPUPacketArena[CurrentBuffer]);
	
	GsClearOt(0,0,&myOT[CurrentBuffer]);
	
	DrawSync(0);

	if(VSync(-1) >= next_second){
		fps=current_frame;
		current_frame=0;
		next_second = VSync(-1) + 60;
	}
	
	VSync(0); //keeps VSync on for 60FPS
	
	GsSwapDispBuff();
	
	GsSortClear(50,50,50,&myOT[CurrentBuffer]);
	
	GsDrawOt(&myOT[CurrentBuffer]);
}

unsigned short voisin(unsigned short x, unsigned short y){
	unsigned short nb_voisin = 0;
	unsigned short right;
	unsigned short left;
	unsigned short down;
	unsigned short up;
	
	if(x-1<0){
		left = SQUARE_NB_X - abs(x-1);
	}
	else{
		left = (x-1)%SQUARE_NB_X;
	}
	
	if(y-1<0){
		up = SQUARE_NB_Y - abs(y-1);
	}
	else{
		up = (y-1)%SQUARE_NB_Y;
	}
	
	if(x+1>SQUARE_NB_X){
		right = x-SQUARE_NB_X;
	}
	else{
		right = (x+1)%SQUARE_NB_X;
	}
	
	if(y+1>SQUARE_NB_Y){
		down = y-SQUARE_NB_Y;
	}
	else{
		down = (y+1)%SQUARE_NB_Y;
	}
	
	if(state[left][down] == LIVE){
		nb_voisin++;
	}

	if(state[x][down] == LIVE){
		nb_voisin++;
	}
	
	if(state[right][down] == LIVE){
		nb_voisin++;
	}
	
	if(state[left][y] == LIVE){
		nb_voisin++;
	}
	
	if(state[right][y] == LIVE){
		nb_voisin++;
	}
	
	if(state[left][up] == LIVE){
		nb_voisin++;
	}
	
	if(state[x][up] == LIVE){
		nb_voisin++;
	}
	
	if(state[right][up] == LIVE){
		nb_voisin++;
	}
	return nb_voisin;
}

void calc(){
	unsigned short i;
	unsigned short j;
	
	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0; j<SQUARE_NB_Y; j++){
			unsigned short nb_voisin = voisin(i,j);
			
			//ISOLATED
			if(state[i][j]==LIVE && nb_voisin<2){
				temp[i][j]=DEAD;
			}
			//SURVIVE
			if(state[i][j]==LIVE && (nb_voisin==2 || nb_voisin==3)){
				temp[i][j]=LIVE;
			}
			//OVERPOPULATION
			if(state[i][j]==LIVE && nb_voisin>3){
				temp[i][j]=DEAD;
			}
			//BIRTH
			if(state[i][j]==DEAD && nb_voisin==3){
				temp[i][j]=LIVE;
			}
		}
	}
	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0; j<SQUARE_NB_Y; j++){
			state[i][j] = temp[i][j];
			selector_field[i][j] = state[i][j];
		}
	}
}

void draw(){
	unsigned short i;
	unsigned short j;
	
	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0; j<SQUARE_NB_Y; j++){
			if(state[i][j]==DEAD){
				updateColorSQUARE(&cells[i][j], 255, 255, 255);
			}
			else if(state[i][j]==LIVE){
				updateColorSQUARE(&cells[i][j], 0, 0, 0);
			}
		}
	}
	selector_status(&selector);
	
	if(stepMode==0){
		updateColorSQUARE(&background, 0, 255, 0);
	}
	if(stepMode==1){
		updateColorSQUARE(&background, 255, 255, 0);
	}
}

void pad_read(){
	int padd = PadRead(1);
	
	unsigned short i=0;
	unsigned short j=0;
	
	if(padd & PADL1){
		if(padL1Down==0){
			padL1Down = 1;
			if(stepMode==0){
				stepMode=1;
			}
			else if(stepMode==1){
				stepMode=0;
			}
		}
	}
	else{
		if(padL1Down==1){
			padL1Down=0;
		}
	}
	if(stepMode==1){
		direction_step(padd);
	}
	else if(stepMode==0){
		direction(padd);
	}
	
	//Pause State
	if(padd & PADstart){
		if(padStartDown==0){
			padStartDown=1;
			if(isPaused==0){
				isPaused=1;
			}
			else if(isPaused==1){
				isPaused=0;
			}	
		}
	}
	else{
		if(padStartDown==1){
			padStartDown=0;
		}
	}
	
	//LIVE/DEAD status step
	if(padd & PADRdown){
		if(padXDown==0){
			padXDown=1;
			if(state[selector.x][selector.y]==DEAD){
				state[selector.x][selector.y]=LIVE;
			}
			else{
				state[selector.x][selector.y]=DEAD;
			}
		}
	}
	else{
		if(padXDown==1){
			padXDown=0;
		}
	}
	//LIVE/DEAD status chain
	if(padd & PADRup){
		if(state[selector.x][selector.y]==DEAD){
			state[selector.x][selector.y]=LIVE;
		}
		else{
			state[selector.x][selector.y]=DEAD;
		}
	}
	
	//Step
	if(isPaused==1){
		if(padd & PADR1){
			if(padR1Down==0){
				padR1Down=1;
				calc();
			}
		}
		else{
			if(padR1Down==1){
				padR1Down=0;
			}
		}
	}
	
	//Clean
	if(isPaused==1){
		if(padd & PADselect){
			for(i=0; i<SQUARE_NB_X; i++){
				for(j=0;j<SQUARE_NB_Y; j++){
					initSQUARE(&cells[i][j], SQUARE_SIZE);
					placeSQUARE(&cells[i][j], (12+i*SQUARE_SIZE), (12+j*SQUARE_SIZE));
					state[i][j] = DEAD;
					temp[i][j] = DEAD;
				}
			}
		}
	}
}

void selector_status(SELECTOR* selector){
	if (state[selector->x][selector->y]==DEAD){
		updateColorSQUARE(&selector->sprite, 255, 0, 0);
	}
	else{
		updateColorSQUARE(&selector->sprite, 0, 0, 255);
	}
}

void direction(int padd){
	//Going Up
	if(padd & PADLup){
		if(selector.y-1<0){
			selector.y = SQUARE_NB_Y-1;
			translateSQUARE_Vertical(&selector.sprite, (SQUARE_NB_Y-1)*SQUARE_SIZE);
		}
		else{
			selector.y--;
			translateSQUARE_Vertical(&selector.sprite, -SQUARE_SIZE);
		}
	}
	
	//Going Down
	if(padd & PADLdown){
		if(selector.y+1>SQUARE_NB_Y-1){
			selector.y = 0;
			translateSQUARE_Vertical(&selector.sprite, -(SQUARE_NB_Y-1)*SQUARE_SIZE);
		}
		else{
			selector.y++;
			translateSQUARE_Vertical(&selector.sprite, SQUARE_SIZE);
		}
	}
	
	//Going Left
	if(padd & PADLleft){
		if(selector.x-1<0){
			selector.x = SQUARE_NB_X-1;
			translateSQUARE_Horizontal(&selector.sprite, (SQUARE_NB_X-1)*SQUARE_SIZE);
		}
		else{
			selector.x--;
			translateSQUARE_Horizontal(&selector.sprite, -SQUARE_SIZE);
		}
	}
	
	//Going Right
	if(padd & PADLright){
		if(selector.x+1>SQUARE_NB_X-1){
			selector.x = 0;
			translateSQUARE_Horizontal(&selector.sprite, -(SQUARE_NB_X-1)*SQUARE_SIZE);
		}
		else{
			selector.x++;
			translateSQUARE_Horizontal(&selector.sprite, SQUARE_SIZE);
		}
	}
}
void direction_step(int padd){
	//Going Up
	if(padd & PADLup){
		if(padUpDown==0){
			padUpDown=1;
			if(selector.y-1<0){
				selector.y = SQUARE_NB_Y-1;
				translateSQUARE_Vertical(&selector.sprite, (SQUARE_NB_Y-1)*SQUARE_SIZE);
			}
			else{
				selector.y--;
				translateSQUARE_Vertical(&selector.sprite, -SQUARE_SIZE);
			}
		}
	}
	else{
		if(padUpDown==1){
			padUpDown=0;
		}
	}
	
	//Going Down
	if(padd & PADLdown){
		if(padDownDown==0){
			padDownDown=1;
			if(selector.y+1>SQUARE_NB_Y-1){
				selector.y = 0;
				translateSQUARE_Vertical(&selector.sprite, -(SQUARE_NB_Y-1)*SQUARE_SIZE);
			}
			else{
				selector.y++;
				translateSQUARE_Vertical(&selector.sprite, SQUARE_SIZE);
			}
		}
	}
	else{
		if(padDownDown==1){
			padDownDown=0;
		}
	}
	
	//Going Left
	if(padd & PADLleft){
		if(padLeftDown==0){
			padLeftDown=1;
			if(selector.x-1<0){
				selector.x = SQUARE_NB_X-1;
				translateSQUARE_Horizontal(&selector.sprite, (SQUARE_NB_X-1)*SQUARE_SIZE);
			}
			else{
				selector.x--;
				translateSQUARE_Horizontal(&selector.sprite, -SQUARE_SIZE);
			}
		}
	}
	else{
		if(padLeftDown==1){
			padLeftDown=0;
		}
	}
	
	//Going Right
	if(padd & PADLright){
		if(padRightDown==0){
			padRightDown=1;
			if(selector.x+1>SQUARE_NB_X-1){
				selector.x = 0;
				translateSQUARE_Horizontal(&selector.sprite, -(SQUARE_NB_X-1)*SQUARE_SIZE);
			}
			else{
				selector.x++;
				translateSQUARE_Horizontal(&selector.sprite, SQUARE_SIZE);
			}
		}
	}
	else{
		if(padRightDown==1){
			padRightDown=0;
		}
	}
}

void init(){
	unsigned short i;
	unsigned short j;
	const VAGhdr_t *VAGhdr = (VAGhdr_t *)VAGaddr;
	DWORD pitch = (SWAP_ENDIAN32(VAGhdr->samplingFrequency) <<12)/44100L;

	PadInit(0);
	SpuInit();
	InitMusicSystem();
	FntLoad(960,256);
	SetDumpFnt(FntOpen(5,20,SCREEN_WIDTH,SCREEN_HEIGHT,0,512));
	
	SendVAGToRAM(SWAP_ENDIAN32(VAGhdr->dataSize));

	SetVoiceAttr(pitch);
	
	graphics();
	
	for(i=0; i<SQUARE_NB_X; i++){
		for(j=0;j<SQUARE_NB_Y; j++){
			initSQUARE(&cells[i][j], SQUARE_SIZE);
			placeSQUARE(&cells[i][j], (12+i*SQUARE_SIZE), (12+j*SQUARE_SIZE));
			state[i][j] = DEAD;
			temp[i][j] = DEAD;
		}
	}
	
	initSQUARE(&selector_sprite, SQUARE_SIZE);
	initSELECTOR(&selector, 0, 0, 0, selector_sprite);
	placeSQUARE(&selector.sprite, 12, 12);
	
	initSQUARE(&background, 320);
	placeSQUARE(&background, 0, 0);
	updateColorSQUARE(&background, 0, 255, 0);
}
