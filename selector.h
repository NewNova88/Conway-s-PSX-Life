#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "square.h"

/*
	SELECTOR structure for manual cells
	selection
*/
typedef struct SELECTOR{
	SQUARE sprite;
	int x;
	int y;
	unsigned int status;
	
}SELECTOR;

void initSELECTOR(SELECTOR* selector, int x, int y, unsigned int status, SQUARE sprite);
void updateSELECTOR(SELECTOR* selector, int x, int y, unsigned int status, SQUARE sprite);

#endif
