#include "selector.h"

void initSELECTOR(SELECTOR* selector, int x, int y, unsigned int status, SQUARE sprite){
	selector->x = x;
	selector->y = y;
	selector->status = status;
	selector->sprite = sprite;
}

void updateSELECTOR(SELECTOR* selector, int x, int y, unsigned int status, SQUARE sprite){
	selector->x = x;
	selector->y = y;
	selector->status = status;
	selector->sprite = sprite;
}
