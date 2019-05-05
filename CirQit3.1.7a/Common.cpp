#include "Common.h"
#include <sstream>

void error(const char* description)
{
	cerr << "ERROR: " << description << endl;
}


// Helper function: convert an integer to string
string toSt(int i)
{
	stringstream s;
	s << i;
	string result;
	s >> result;
	return result;
}


string getNewName()
{
	return "tmp" + toSt(Common::tmp++);
}


bool Common::larify = false;
long Common::backtracks = 0;

bool Common::phase = true;
bool Common::phase_exist = true;

bool Common::preprocess = true;

int Common::qpro = 1;

bool Common::do_proof = false;
ofstream Common::proof_file;

int Common::tmp = 0;

int Common::value = VALUE_UNASSIGNED;

clock_t Common::start_time;
unsigned Common::timeout;

