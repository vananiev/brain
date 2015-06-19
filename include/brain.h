#ifndef __BRAIN_H__
#define __BRAIN_H__

#include "cell.h"
#include <iostream>
#include <pthread.h>
#include <exception>

using namespace std;


class Brain{

private:
	istream *_receptor;
	ostream *_action;
	vector< Cell > _receptor_cell;
	vector< Cell > _cell;
	vector< Cell > _pleasure_cell;
	vector< Cell > _pleasure_wait_cell;
	vector< Cell > _action_cell;
	unsigned int active_receptor_index; // В какой рецептор отсылаются данные
	int  _cant_create_pleasure_wait;

	void brain_io();
	static void * brain_io_thread_func(void* brain);
	pthread_t _brain_io_thread;
	bool read_receptor();
	void write_action();
	void print_cells(vector<Cell>&, const char* msg);
	
	void kill();

	void execute();
	static void * execute_thread_func(void* brain);
	pthread_t _execute_thread;
	void do_for_pleasure();
	void do_rand_action();
	void check_pleasure();
	void enhance_synapses();
	void enfeeble_synapses();
	int get_current_active_cell_cnt(vector<Cell>& cells);
	void print_action();
	void print_synapses(vector<Cell>&);

	void calc_activity_in_time();
	void calc_activity_for(vector<Cell>&);

	void erase_dead_cells();
	void erase_dead_cells_for(vector<Cell>&);

public:
	Brain(istream *receptor, ostream *action);
	~Brain();
	void wait_death();

};

class BrainException : public exception {
private:
	const char* _msg;
public:
	BrainException(const char* msg){ _msg=msg;}
	virtual const char* what() const throw(){
		return _msg;
	}
};


#endif 
