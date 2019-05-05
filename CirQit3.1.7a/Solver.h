#ifndef SOLVER_H
#define SOLVER_H

#include "DagNode.h"
#include "Circuit.h"
#include <stack>
#include "PropRecord.h"
#include "time.h"
#include "UndoRecord.h"
#include "LearningDB.h"
// (*) Prune include lists to remove anything unnecessary

#define TIME_OUT_ERROR -1

class Solver
{
	friend class BlockChangeUndo;
	friend class UndoRecord;

protected:
	Circuit *circuit;

	// True iff need to output log dump
	bool verbose; // (*) Check all the status messages TODO

	// The time that the current solving session began
	clock_t solver_start_time;
	// The time until throwing time out exception
	unsigned solver_time_out;

	LearningDB db;

	short last_type; // Which tree the last conflict was on?

	/**************************************************************************
	* Decision levels
	**************************************************************************/
	// The number of the current decision level
	int d_level;

	// For reporting, the maximum depth ever reached:
	long max_depth;
	// For reporting, the number of decision nodes:
	long decisions_made;

	// The current quantifier block; is used for chooseNextLiteral
	int curr_block;

	// (*) Comment
	// (*) Consider making this a template
	stack<int>undo_ptrs;
	int undo_ptr;

public:
	UndoStack undo_stack;

private:
	// (*) Consider a better way of doing reporting solutions TODO
	// because updating these every time is very inefficient
	// for reporting solutions
	vector<DagNode*> decision_vars;
	vector<int> decision_values;


	// The container for all the propagations that are not yet processed
	// This stack is empty iff all unit propagation that could be done
	// Has been done
	UPstack up_stack;
	stack<DagNode*> justified_stack; //(*) consider moving into Circuit
	stack<short> justified_types;


	/**************************************************************************
	* Functions for solving
	**************************************************************************/

	// Create a new decision level with the given 
	// assignment (variable=value) as the decision
	// force indicates whether or not to force this assignment.
	// This should be true for root_node, false otherwise
	void newDecisionLevel( DagNode * variable, int value, bool force = false);
	// Undo all the changes and backtrack to the last decision level
	void undoDecisionLevel();

	// Choose the next assignment and put it into provided variables
	bool chooseNextLiteral( DagNode *& node, int &value);


	// Do unit propagation: propagate all changes that are in the up_stack,
	//   repeat until up_stack is empty
	int unitPropagate();
	int propagateJustified();

public:
	Solver(Circuit *c, bool verbose, unsigned time_out = (unsigned) - 1);

	// Returns the current decision level
	int getDLevel() {return d_level;};

	int getTrailPos() {return undo_stack.size();};

	bool solve();

	// Reset stacks for the next iteration
	void resetStacks();

	void printStats();

	/**************************************************************************
	* Learning
	**************************************************************************/

	// (*) Temporary: all this might be replaced by dual learning
	
struct cube_data {
    int score;
    int index;
};
	//map<DagNode*, cube_data> cube_scores;
	//void findMinSufficientSet( DagNode * curr, int * score, int * index );
	//void recoverCube( DagNode * node );
	//int getCubeScore( DagNode * node );
	//void findCube();
};

#endif
