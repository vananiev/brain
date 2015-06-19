
#include <iostream>
#include "brain.h"

using namespace std;


int main(int argc, char** argv){
	try{

		Brain b(&cin, &cout);
		b.wait_death();

	} catch( BrainException* e){
		cerr << e->what() << endl;
		delete e;
		return 1;
	} catch ( ... ){
		cerr << "Unknown exception" << endl;
		return 1;
	}

	return 0;

} 
