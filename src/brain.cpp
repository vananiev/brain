#include "brain.h"
#include <sstream>
#include <string>
#include <unistd.h>
#include <cstdlib>



using namespace std;

Brain::Brain(istream *receptor, ostream *action){
	_receptor = receptor;
	_action = action;
	_brain_io_thread = 0;
	_execute_thread = 0;
	active_receptor_index = 0;
	_cant_create_pleasure_wait = 0;

	_receptor_cell.resize(4);
	_cell.resize(4);
	_pleasure_cell.resize(1);
	_action_cell.resize(4);

	for(int i=0; i<1; i++)
		_receptor_cell.at(i).enhance_synapse_to(&_pleasure_cell.at(i), 50);

	srand(time(NULL));
	execute();
	brain_io();

}

Brain::~Brain(){
	kill();
}



bool Brain::read_receptor(){

	char str[1024];
	stringstream ss;

	cerr << endl << active_receptor_index <<"> ";
	(*_receptor) >> str;
	
	switch( str[0]){
	case '.':
		return false;
	case 'p':
		print_synapses(_pleasure_wait_cell);
		return true;
	}

	char* ind = str;
	if(str[0] == '='){
		ind++;
		ss.str( ind );
		ss >> active_receptor_index;
		cerr << endl << active_receptor_index <<"> ";
		(*_receptor) >> str;
	}

	if( active_receptor_index < 0 or active_receptor_index >= _receptor_cell.size())
		throw new BrainException("Receptor index is wrong");

	int activity = 0;
	ss.clear();
	ss.str( str );
	ss >> activity;
	_receptor_cell.at(active_receptor_index).set_activity( activity );

	return true;

}



void Brain::write_action(){

	print_cells(_receptor_cell, "Receptors:");
	print_cells(_pleasure_wait_cell, "Pleasure wait:");

	cerr << "Action: " << endl;
	for(int i=0, cnt=_action_cell.size(); i<cnt; i++){
		(*_action) << "=" << i << endl;
		(*_action) << _action_cell.at(i).get_activity() << endl;
	}

	print_cells(_pleasure_cell, "Pleasure:");

}




void Brain::print_cells(vector<Cell>& c, const char* msg){
	cerr << msg << " " << c.size() << endl;
	for(int i=0, cnt=c.size(); i<cnt; i++){
		cerr << "=" << i << endl;
		cerr << c.at(i).get_activity() << endl;
	}
}



void Brain::print_synapses(std::vector<Cell> &c){
	
	for(int i=0, i_cnt=c.size(); i<i_cnt; i++){
		cerr  << i << " {" << endl;
		c.at(i).print_synapses();
		cerr << "}" << endl;
	}
}


void* Brain::brain_io_thread_func(void* brain){

	bool brain_live = true;

	while( brain_live ){
		brain_live = ((Brain*)brain)->read_receptor();
		((Brain*)brain)->write_action();
	}

	return NULL;

}



void Brain::brain_io(){
	if (
		pthread_create(
			&_brain_io_thread,
			NULL,
			brain_io_thread_func,
			this
		) != 0
	)
		throw new BrainException("Can't run brain_io() brain is dead");
	
}



void Brain::kill(){
	pthread_cancel( _brain_io_thread );
	pthread_cancel( _execute_thread );
}



void Brain::wait_death(){
	pthread_join( _brain_io_thread, NULL );
}



void Brain::execute(){

	if (
		pthread_create(
			&_execute_thread,
			NULL,
			execute_thread_func,
			this
		) != 0
	)
		throw new BrainException("Can't run execute() brain is dead");
}


void* Brain::execute_thread_func(void* b){

	Brain* brain = (Brain*) b;
	while(true){
		brain->do_for_pleasure();
		brain->check_pleasure();
		brain->calc_activity_in_time();
		brain->erase_dead_cells();
		brain->print_action();

	}

}



void Brain::print_action(){

	if( CELL_ACTIVE_LEVEL < _action_cell.at(0).get_activity() and 
		CELL_ACTIVE_LEVEL > abs(_action_cell.at(1).get_activity())
		)
		cerr << "Eat" << endl;

	if( CELL_ACTIVE_LEVEL < _action_cell.at(1).get_activity()  and 
		CELL_ACTIVE_LEVEL > abs(_action_cell.at(0).get_activity()) )
		cerr << "Don't move" << endl;

}



void Brain::do_for_pleasure(){

	if( 0 == _pleasure_wait_cell.size() ){
		do_rand_action();
		return;
	}

	int choice = 0;
	if( get_current_active_cell_cnt(_action_cell) > 0 )
		choice = rand()%10;
	else
		choice = 1;

	int ind = 0, activity = 0;
	switch( choice ){
	case 0:
		ind = rand()%_pleasure_wait_cell.size();
		activity = rand() % ( 100 );
		//cerr << "PW activate for " << ind << " = " << activity << endl;
		_pleasure_wait_cell.at(ind).set_activity( activity );
		break;
	case 1:
		do_rand_action();
		return;
	default:
		return;
	}

}


void Brain::do_rand_action(){
	//cerr << "do_rand_action" << endl;
	if( get_current_active_cell_cnt(_action_cell) > 0 )
		return;
	//if( rand()%10 )
	//	return; // only x% times action is perfomed

	_action_cell.at(
			rand()%_action_cell.size()
		).set_activity( 100 );

}



int Brain::get_current_active_cell_cnt(vector<Cell>& cells){

	int ret = 0;
	for(int i=0, cnt=cells.size(); i<cnt; i++){
		if( CELL_ACTIVE_LEVEL < abs(cells.at(i).get_activity()) )
			ret++;
	}
	return ret;

}



void Brain::check_pleasure(){
	if( get_current_active_cell_cnt(_pleasure_cell) )
		enhance_synapses();
	else
		enfeeble_synapses();
}



void Brain::enfeeble_synapses(){
	//cerr << "No pleasure" << endl;
	for(vector<Cell>::iterator i = _pleasure_wait_cell.begin(); i!=_pleasure_wait_cell.end(); i++){
		if( CELL_ACTIVE_LEVEL < abs(i->get_activity()) ){
			i->enhance_synapse_to_active_cells(0.9);
		}
	}
}



void Brain::enhance_synapses(){
	//cerr << " Pleasure" << endl;
	Cell* pleasure_wait_cell = NULL;

	for(int i=0, cnt=_pleasure_wait_cell.size(); i<cnt; i++){
		if( CELL_ACTIVE_LEVEL > abs(_pleasure_wait_cell.at(i).get_activity()) ){
			pleasure_wait_cell = &_pleasure_wait_cell.at(i);
			break;
		}
	}

	if( !pleasure_wait_cell ){
		_cant_create_pleasure_wait++;
		if( _cant_create_pleasure_wait > 0 ){
			cerr << "Pleasure wait cell is created" << endl;
			_pleasure_wait_cell.resize( _pleasure_wait_cell.size() + 1 );
			_cant_create_pleasure_wait = -100;
		}
		pleasure_wait_cell = &_pleasure_wait_cell.back();
	}

	for(vector<Cell>::iterator i = _receptor_cell.begin(); i!=_receptor_cell.end(); i++){

		if( CELL_ACTIVE_LEVEL < abs(i->get_activity()) )
			continue;

		//cerr << "PW -> R" << endl;
		pleasure_wait_cell->enhance_synapse_to(&(*i));

		for(vector<Cell>::iterator j = _action_cell.begin(); j!=_action_cell.end(); j++){
			if( CELL_ACTIVE_LEVEL < abs( j->get_activity() ) )
				continue;
			//cerr << "PW -> A, R -> A, A-> R" << endl;
			i->enhance_synapse_to(&(*j));
			j->enhance_synapse_to(&(*i));
			pleasure_wait_cell->enhance_synapse_to(&(*j));
		}

	}
}



void Brain::calc_activity_in_time(){
	usleep(300000);
	calc_activity_for( _receptor_cell );
	calc_activity_for( _cell );
	calc_activity_for( _pleasure_cell );
	calc_activity_for( _pleasure_wait_cell );
	calc_activity_for( _action_cell );
}



void Brain::calc_activity_for(vector<Cell>& c){

	for(int i=0, i_cnt=c.size(); i<i_cnt; i++){
		c.at(i).execute();
	}

}




void Brain::erase_dead_cells(){
	//erase_dead_cells_for( _cell );
	erase_dead_cells_for( _pleasure_wait_cell );
}



void Brain::erase_dead_cells_for(vector<Cell>& c){

	vector<Cell>::iterator it = c.begin();

	for(; it != c.end(); ){
		if( it->is_dead() ){
			cerr << "dead cell is erased" << endl;
			it = c.erase(it);
		}else
			++it;
	}

}