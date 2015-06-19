#include "synapse.h"
#include "cmath"
#include <iostream>

using namespace std;

Synapse::Synapse(){
	_neighbor = NULL;
	_force = 1e-6;
}



void Synapse::enhance_force( float index ){

	if( abs(_force) < 1e-2 ){
		if( _force > 0) _force = 1e-2;
		else	_force = -1e-2;
	}

	_force *= index;

	/*if (index > 1)
		cerr << "Synapse enforsed to " << _force << endl;
	else
		cerr << "Synapse reduced to " << _force << endl;
	*/
}


void Synapse::set_neighbor(Cell* c){
	_neighbor = c;
}



Cell* Synapse::get_neighbor(){
	return _neighbor;
}



void Synapse::set_force(Force f){
	_force = f;
}



Force Synapse::get_force(){
	return _force;
}



void Synapse::send_signal(Activity act){
	_neighbor->input_activity( _force * act );
}
