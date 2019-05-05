#ifndef _dDUMMY_TREE_
#define _dDUMMY_TREE_

#include "Common.h"
#include <vector>
#include <algorithm>
#include "Circuit.h"

#define DUMMY_LEAF 0


class IntSet
{
	vector<int> values;
public:
	IntSet(int max) : values(max)
	{
		clear(max);
	}

	void clear(int newMax)
	{
		int oldMax = values.size();
		if(newMax > oldMax)
		{
			values.resize(newMax);
		}

		for(int i = 0; i < values.size(); i++)
		{
			assert(i >= oldMax || values[i] == 0);
			values[i] = 0;
		}
	}

	void add(int i, bool allowZero = false)
	{
		if(!allowZero)
			assert(i != 0);
		else
		{
			if(i == 0)
				return;
		}

		assert(i * values[abs(i)] >= 0); // Only same polarity can be added
		int add = 1;
		if(i < 0)
			add = -1;
		values[abs(i)] += add;
	}

	void del(int i, bool allowZero = false)
	{
		if(!allowZero)
			assert(i != 0);
		else
		{
			if(i == 0)
				return;
		}

		assert(i * values[abs(i)] > 0); // Only same polarity can be removed
		int add = 1;
		if(i < 0)
			add = -1;
		values[abs(i)] -= add;
	}

	// Return true if the assignments contain the literal of
	// the variable (its negation iff negated)
	int value(int i, bool allowZero = false)
	{
		if(!allowZero)
			assert(i != 0);
		else
		{
			if(i == 0)
				return VALUE_UNASSIGNED;
		}

		int value;
		if(values[abs(i)] < 0)
			value = VALUE_FALSE;
		else if (values[abs(i)] > 0)
			value = VALUE_TRUE;
		else
			value = VALUE_UNASSIGNED;

		if(i < 0 && value != VALUE_UNASSIGNED)
			return opposite(value);
		else
			return value;
	}
};

class DummyTreeNode
{
public:
	// ----Structural bookkeeping-----
	vector<int> leafs;
	vector<DummyTreeNode*> children;
	int type;
	DummyTreeNode *parent;

	int qGates; // used in nameFunctions: non-zero if the subtree has qGates

	int dummyValue;

	// Only relevant for AND and OR gates:
	// which value propagates up, and which propagates down
	int up_value;
	int down_value;

	DummyTreeNode(int ntype)
	{
		parent = NULL;
		type = ntype;
		dummyValue = VALUE_UNASSIGNED;

			
		if(type == AND_GATE)
		{
			up_value = VALUE_FALSE;
			down_value = VALUE_TRUE;
		}
		else if(type == OR_GATE)
		{
			up_value = VALUE_TRUE;
			down_value = VALUE_FALSE;
		}
		else
		{
			up_value = VALUE_UNASSIGNED;
			down_value = VALUE_UNASSIGNED;
		}

		thisFunc = 0;
		negFunc = 0;

		qGates = 0;
	};

	void addChild(int i) {leafs.push_back(i);}

	void addChild(DummyTreeNode *i) 
	{
		children.push_back(i);
		i->parent = this;
	}

	// Delete all children
	void wipeOut()
	{
		for(int i = 0; i < children.size(); i++)
		{
			children[i]->wipeOut();
			delete(children[i]);
		}
	}

	void fixLeafs()
	{
		// Fix Leafs
		int i,j;
		for(i=0,j=0; j<leafs.size(); j++, i++ )
		{
			if(leafs[i] == DUMMY_LEAF)
			{
				while(j < leafs.size() && leafs[j] == DUMMY_LEAF)
				{
					j++;
				}
				if(j < leafs.size())
				{
					leafs[i] = leafs[j];
					leafs[j] = DUMMY_LEAF;
				}
				else
					break;
			}
		}
		while(leafs.size() > i)
		{
			assert(leafs[leafs.size() - 1] == DUMMY_LEAF);
			leafs.pop_back();
		}

		// Fix Children
		for(i=0,j=0; j<children.size(); j++, i++ )
		{
			if(children[i] == NULL)
			{
				while(j < children.size() && children[j] == NULL)
				{
					j++;
				}
				if(j < children.size())
				{
					children[i] = children[j];
					children[j] = NULL;
				}
				else
					break;
			}
		}

		while(children.size() > i)
		{
			assert(children[children.size() - 1] == NULL);
			children.pop_back();
		}
	}

	bool empty() 
	{
		return leafs.size() + children.size() == 0;
	}

	// Put the leave names into the list
	void listLeaves(vector<string> &list);

	// Fill c with information from this tree
	void makeCircuit(Circuit &c, string name, IntSet& assignments);

	void sortChildren();

	// Look for simplifications that can be made
	// by checking downwards
	// Return true iff any changes were made
	bool checkDown(IntSet &assignments);

	// Propagate the constant values set in checkDown
	// (dummyValue)
	int fixTree();

	// Will give each unique subcircuit a unique number
	// Structurally equivalent subcircuits will have the same numbers.
	// thiFunc - will be the function number of this subcircuit
	// negFunc - will be the function number of negation of this subcircuit
	// In both cases, 0 when not initialized or doesn't exist.
	void nameFunctions(map<long, vector<DummyTreeNode*> > &hashMap, vector<DummyTreeNode *> &functions, int vars);
	int thisFunc;
	int negFunc; // Will be filled in only if a negated func exists in the tree
	// get this node's hash value, assuming all children have thisFunc
	// and negFunc initialized. If negate, give hash value of the node
	// opposite to this one.
	long hashValue(bool negate, int vars);
};

int dummyTreePreproc(DummyTreeNode* node, int vars, int *allVars);

#endif
