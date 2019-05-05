#include "DagNode.h"
#include "Solver.h"
#include "UndoRecord.h"

#ifndef PROP_RECORD_H
#define PROP_RECORD_H

class Solver; //(*)

class PropRecord
{
  friend class UPstack;
  protected:
    Signal value;
    DagNode * variable;
    Reason * reason;
    bool forced;
    
  public:  
    PropRecord();
	void set(DagNode * variable, Signal value, Reason * reason, bool forced );
    int propagate1(Solver *solver, bool &ignore);
    
};

class UPstack
{
private:
	static const int num_first, num_inc; // how much to allocate first, and by how much to increment
	vector<PropRecord> data;
	int upTo;

public:
	UPstack();
	inline int next();
	void addNew( DagNode * variable, Signal value, Reason * reason, bool forced );
	bool empty();
	int size();
	int propAndPop(Solver *solver);
	void clear();
};


inline void releaseRecord(PropRecord * rec)
{
	delete(rec);
}

#endif

