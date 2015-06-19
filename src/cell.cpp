#include "cell.h"
#include <iostream>
#include <cmath>

using namespace std;


Cell::Cell(){
	_activity = 0;
}



Activity Cell::get_activity(){
	return _activity;
}



void Cell::set_activity(Activity act){
	_activity = act;
}



void Cell::input_activity(Activity a){
	_activity += a;
}



void Cell::enhance_synapse_to( Cell* cell, float index ){

	int synapse_index = -1;
	for(int i=0, cnt=_synapse.size(); i<cnt; i++){
		if( _synapse[i].get_neighbor() == cell ){
			synapse_index = i;
			break;
		}
	}

	Synapse *syn;
	if( -1 != synapse_index){
		syn = &_synapse.at( synapse_index );
	}else{
		Synapse s;
		s.set_neighbor( cell );
		_synapse.push_back(s);
		syn = &_synapse.back();
	}
	
	syn->enhance_force( index );

}

void Cell::enhance_synapse_to_active_cells(float index){
	for(int i=0, cnt=_synapse.size(); i<cnt; i++){
		Synapse *syn = &_synapse[i];
		if( CELL_ACTIVE_LEVEL < syn->get_neighbor()->get_activity() ){
			syn->enhance_force( index );
		}
	}	
}


void Cell::send_activity_to_synapses(){

	for(int i=0, cnt=_synapse.size(); i<cnt; i++){
		_synapse[i].send_signal( _activity );
	}

}



void Cell::execute(){
	reduce_activity();
	send_activity_to_synapses();
	weak_synapses_die_off();
}



void Cell::reduce_activity(){
	_activity *= 0.8;
}



void Cell::weak_synapses_die_off(){
	vector<Synapse>::iterator it = _synapse.begin();
	for( ; it != _synapse.end(); ){
		if( abs(it->get_force()) < 1e-4 )
			it = _synapse.erase(it);
		else
			++it;
	}
}



bool Cell::is_dead(){
	if( _synapse.size() )
		return false;
	else
		return true;
}



void Cell::print_synapses(){

	vector<Synapse>::iterator it = _synapse.begin();
	for( ; it != _synapse.end(); it++){
		cerr << " ->" << it->get_force() << endl;
	}
}