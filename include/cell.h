#ifndef __CELL_H__
#define __CELL_H__

typedef int Activity;

#include <vector>
#include "synapse.h"

#define CELL_ACTIVE_LEVEL 5

using namespace std;

class Synapse;

class Cell{

private:
	Activity _activity;	// состояние возбуждения
	vector< Synapse > _synapse;
	void send_activity_to_synapses();	// отправить сигнал в синапс
	void reduce_activity();		// Во времени возбуждение ячейки меняется

public:
	Cell();
	void execute();	// просчитать текущее состояние
	void enhance_synapse_to( Cell*, float index=1.1 ); // усилить(>1)/уменьшить(<1) связь с ячейкой
	void enhance_synapse_to_active_cells(float index=1.1); // усилить связь ко всем активным ячейкам
	void weak_synapses_die_off(); // слабые связи отмирают
	void input_activity( Activity );	// Входящий сигнал от другой ячейки
	Activity get_activity();
	void set_activity(Activity);
	void calc_activity_in_time();
	bool is_dead();	// if no synapses then dead
	void print_synapses();
};


#endif