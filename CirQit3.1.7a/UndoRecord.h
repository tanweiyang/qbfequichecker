#ifndef UNDORECORD_H
#define UNDORECORD_H

// (*) Consider a different memory management scheme w/o need for this
// Memory chunks containing all information; to erase just set counter to 0

#include "DagNode.h"


class Solver;

#define UNDO_INVALID -1
#define UNDO_ASSIGNMENT  13
#define UNDO_JUSTIFIED   14
#define UNDO_BLOCK 15

class UndoRecord
{
private:
	short type;
	union
	{
		struct{DagNode* var; short linetype;} assignment;
		struct{int last_block; Solver *solver;} block;
	}
	data;

public:
	UndoRecord(); // Dummy - can't undo until set
    void set(short type, DagNode* var, short linetype);
	void set(int last_block, Solver *solver);
    void undo();
};


class UndoStack
{
private:
	static const int num_first, num_inc; // how much to allocate first, and by how much to increment
	vector<UndoRecord> data;
	int upTo;
public:
	UndoStack();
	inline int next();
	void addNew(short type, DagNode* var, short linetype);
	void addNew(int last_block, Solver *solver);
	bool empty();
	int size();
	void undoAndPop();
};

#endif
