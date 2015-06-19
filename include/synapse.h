#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

typedef float Force;

#include "cell.h"
#include <cstddef>

using namespace std;

class Cell;

class Synapse{
	Cell* _neighbor;	// соседняя ячейка
	Force _force;		// степень влияния

public:
	Synapse();
	void enhance_force( float index );	// усилить/ослабить связь
	void set_neighbor(Cell*);
	Cell* get_neighbor();
	void set_force(Force);
	Force get_force();
	void send_signal(Activity act);

}; 

#endif