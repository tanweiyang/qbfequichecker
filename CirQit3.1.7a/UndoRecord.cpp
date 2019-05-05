

#include "UndoRecord.h"
#include "Solver.h"

/** Assignment undo **/

extern bool verbose;

UndoRecord::UndoRecord()
{
	type = UNDO_INVALID;
}

void UndoRecord::set(short ntype, DagNode* var, short linetype)
{
	type = ntype;
	data.assignment.var = var;
	data.assignment.linetype = linetype;
}

void UndoRecord::set(int last_block, Solver *s)
{
	type = UNDO_BLOCK;
	data.block.last_block = last_block;
	data.block.solver = s;
}


void UndoRecord::undo()
{
	if(type == UNDO_BLOCK)
	{
		data.block.solver->curr_block = data.block.last_block;
	}
	else if(type == UNDO_ASSIGNMENT)
	{
		data.assignment.var->resetValue(data.assignment.linetype);
	}
	else
	{
		assert(type == UNDO_JUSTIFIED);
		data.assignment.var->setJustified( false, data.assignment.linetype, NULL );
	}
}

const int UndoStack::num_first = FIRST_BATCH;
const int UndoStack::num_inc = INCREMENT;

UndoStack::UndoStack() : data(num_first)
{
	upTo = 0;
}

inline int UndoStack::next()
{
	if(data.size() <= upTo)
	{
		data.resize(upTo + num_inc);
	}
	return upTo++;
}

void UndoStack::addNew(short type, DagNode* var, short linetype)
{
	data[next()].set(type, var, linetype);
}

void UndoStack::addNew(int last_block, Solver *solver)
{
	data[next()].set(last_block, solver);
}

bool UndoStack::empty()
{
	return upTo == 0;
}

int UndoStack::size()
{
	return upTo;
}

void UndoStack::undoAndPop()
{
	// When you undo, you can't create an undo record
	assert(!empty());
	data[upTo-1].undo();
	upTo--;
}
