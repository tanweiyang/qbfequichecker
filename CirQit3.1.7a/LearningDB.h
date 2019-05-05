#ifndef LEARNING_DB_H
#define LEARNING_DB_H


#include "Literal.h"
#include "DagNode.h"
#include <vector>
#include <stack>


// private helper class; defined in the cpp
class OrderedNodeList;

class ComReason
{
private:
	short linetype;
	OrderedNodeList *active;
	int universals, existentials; // for current linetype - reversed for cubes(0LINE)

	// The earliest prefix-level of an unset literal in this reason
	// unsetCount is the number of unset literals at the same prefix-level
	// Note: we assume that we can only remove an unset literal when
	// all the prefix-deeper variables have been eliminated.
	// So, on deletion, if unsetCount reaches 0, then there are no unset 
	// variables remaining in the reason.
	// NOTE: unsetCount is the count of unsets AT THAT QUANTIFIER LEVEL,
	//    not total number of unsets
	int firstUnset, unsetCount;

	// Check this reason. If there are any potential
	// problems (universals with opposite values), reduce the relevant
	// universals, so that the way is clear to add the reason
	// without any extra checks
	void addReason(Reason *r, DagNode* implied, int reduceTo);

	// This is a helper function for removeDeepest.
	void removeDeepestLit(bool block, Literal *lit);

	// This should be the only function to remove literals
	// Attempt to remove the deepest literal in this reason
	// If block is true, attempt at the block-deepest.
	// Otherwise, attempt at the trail-deepest
	bool removeDeepest(bool block);

	// Add all the literals in this literal's reason
	void replaceLit(Literal *lit);

	// Check if the valid implicant is found
	bool hasImplicant();

public:
	ComReason();
	~ComReason();
	// Add the literal to this set: can't be used after
	// initialization
	// mustReduce: means that when looking for the Unit Implicant, this 
	// variable must be reduced (treat it as if it was unassigned)
	void add(Literal *lit);

	// Returns TYPE_1LINE (or TYPE_0LINE) if it's a clause (cube)
	int getLineType() {return linetype;};

	// Pass in the result for the next iteration: QSAT or CONFLICT
	void clear(int result, Reason* conflict_clause);

	bool empty();
	bool containsOnly(DagNode* node);

	// Get the number of universals variables in the active part of this reason
	int getUniversals() {return universals;};
	// Get the number of existential variables in the active part of this reason
	int getExistentials() {return existentials;};

	// Create two arrays and fill them with literals of this reason
	// Literals are filled in the order of block_num
	// replaceList are:
	//      existentials for clauses
	//      universals for cubes
	// reduceList are:
	//      universals for clauses
	//      existentials for cubes
	void exportPointers(Literal*** replaceList, Literal*** reduceList);

	/*****************************************************************************
	Functions needed for UIP learning
	*****************************************************************************/

	// Change the learned set such that it has a unit implicant
	// By doing UIP Learning
	// Return the unit implicant of this learned set
	// type is either SET_CLAUSE or SET_CUBE
	//   - this affects which of the universal and existential will get
	//     expanded, and which - reduced
	Literal *getImplicant();

	// Modify this clause to derive an empty one
	void getEmpty();

	// Return the second last decision level in this set
	// (after the implicant is found, this is the level to backtrack to)
	int lastBTLevel();

	void print();
};

class LearnedSet; // declared in the cpp file because it is private 
//                    and shouldn't be accessed from outside
class Solver;

class LearningDB
{
  private:
	vector<LearnedSet *> stored[VAL_TYPES];
	int count[VAL_TYPES];
	int forced_count[VAL_TYPES];
	Circuit *circuit;
	
	stack<LearnedSet *> true_sets;
	stack<LearnedSet *> prop_sets;

  public:
    LearningDB(Circuit *c);
    ~LearningDB();
    void printStats();
	// Add the new set to the database. 
	//   - set is the set to use; the elements will be copied and not modified
	//   - linetype is either TYPE_1LINE or TYPE_0LINE, 
	//     for conflicts and solutions respectively
	void addSet(ComReason *set, int linetype, bool force);

	// the learned set calls this to indicate that it found a conflict or qsat
	int trigger(LearnedSet *l);
	// the learned set calls this to indeicate that it can assign a variable
	int trigger(LearnedSet *l, Literal* implicant);

	int checkStatus();
    void resetDB();
    int propagateDB();



    //void deleteInactiveCubes();
    //void deleteInactiveNoGoods();
};

#endif
