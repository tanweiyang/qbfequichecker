#include "DummyTree.h"


bool compareLeafs(int a, int b)
{
	if(abs(a) < abs(b))
		return true;
	if(abs(a) == abs(b) && a < b)
		return true;
	return false;
}

bool compareDummyNodes(DummyTreeNode *a, DummyTreeNode *b)
{
	assert(a != NULL && b != NULL);
	return a->thisFunc < b->thisFunc;
}

void DummyTreeNode::sortChildren()
{
	sort(leafs.rbegin(), leafs.rend(), compareLeafs);
	sort(children.rbegin(), children.rend(), compareDummyNodes);
}

void DummyTreeNode::listLeaves(vector<string> &list)
{
	for(int i = 0; i < leafs.size(); i++)
	{
		assert(leafs[i] != 0);
		string s = toSt(abs(leafs[i]));
		if(leafs[i] < 0)
			s = "n" + s;
		list.push_back(s);
	}
}

void DummyTreeNode::makeCircuit(Circuit &circuit, string name, IntSet &assignments)
{
	if(type == QA_GATE || type == QE_GATE)
	{
		vector<string> list;
		listLeaves(list);
		int gateType;
		if(type == QA_GATE)
			gateType = UNIVERSAL;
		else
			gateType = EXISTENTIAL;
		circuit.addQuantifier(list, gateType);

		for(int i = 0; i < list.size(); i++)
		{
			vector<string> vec;
			vec.push_back(list[i]);
			circuit.addGate("n" + list[i], NOT_GATE, vec);
		}

		assert(children.size() == 1);
		children[0]->makeCircuit(circuit, name, assignments);
	}
	if(type == AND_GATE || type == OR_GATE || type == NOT_GATE)
	{
		if(assignments.value(thisFunc, true) != VALUE_UNASSIGNED)
		{
			// This node was already created; nothing to do
			int x = 34543 + 345345;
		}
		else if(assignments.value(negFunc, true) != VALUE_UNASSIGNED)
		{
			// The negation of this node has been created.
			// Just use it
			vector<string> list;
			list.push_back("f" + toSt(negFunc));
			circuit.addGate(name, NOT_GATE, list);
			assignments.add(thisFunc, false);
		}
		else
		{
			// Need to create the node
			vector<string> list;
			listLeaves(list);
			for(int i = 0; i < children.size(); i++)
			{
				string nname;
				if(children[i]->thisFunc != 0)
					nname = "f" + toSt(children[i]->thisFunc);
				else
					nname = getNewName();
				children[i]->makeCircuit(circuit, nname, assignments);
				list.push_back(nname);
			}
			circuit.addGate(name, type, list);
			assignments.add(thisFunc, true);
		}
	}
}


bool DummyTreeNode::checkDown(IntSet &assignments)
{
	Common::checkTime();

	bool result = false;
	int i;

	if(dummyValue != VALUE_UNASSIGNED)
		return false; // It's a constant - nothing to do

	sortChildren();

	if(type == QA_GATE || type == QE_GATE || type == NOT_GATE)
	{
		return children[0]->checkDown(assignments); // Nothing to do
	}

	assert(type == AND_GATE || type == OR_GATE);

	int multiple = 1;
	bool negate = false;
	if(type == OR_GATE)
	{
		multiple = -1;
		negate = true;
	}

	// Go through the leafs.
	// Check if any leaf can be deleted or propagated;
	// If not, add it to the list of future propagations.
	for(i=0; i < leafs.size(); i++)
	{
		if(leafs[i] == DUMMY_LEAF)
			continue;

		int val = assignments.value(leafs[i]);
		if(val == VALUE_UNASSIGNED)
		{
			assignments.add(leafs[i] * multiple);
		}
		else if(val == down_value)
		{
			result = true;
			// Value to delete the leaf
			leafs[i] = DUMMY_LEAF;
		}
		else if(val == up_value)
		{
			// Propagate the value
			dummyValue = up_value;
			for(int j = 0; j < i; j++)
			{
				if(leafs[j] != DUMMY_LEAF)
					assignments.del(leafs[j]*multiple);
			}
			return true;
		}
		else
		{
			assert(false); // There are no other valid options!
		}
	}


	// Check if any child can be deleted or propagated;
	// If not, add it to the list of future propagations.
	// Note: will do recursive calls later.
	for(i=0; i < children.size(); i++)
	{
		if(children[i] == NULL)
			continue;

		assert(children[i]->thisFunc != 0 || qGates > 0);

		int val = assignments.value(children[i]->thisFunc, true);
		int valNeg = assignments.value(children[i]->negFunc, true);

		// We add either thisFunc or negFunc - never as a negative literal
		if(val != VALUE_UNASSIGNED)
			assert(val == VALUE_TRUE);

		if(valNeg != VALUE_UNASSIGNED)
		{
			// Can only have one polarity of a function.
			assert(valNeg == VALUE_TRUE);
			assert(val == VALUE_UNASSIGNED);
			val = VALUE_FALSE;
		}

		if(val == VALUE_UNASSIGNED)
		{
			// Since we can't have recursive functions,
			//      no child will match its own descendant,
			//      so we don't have to worry about it
			if(negate)
				assignments.add(children[i]->negFunc, true);
			else
				assignments.add(children[i]->thisFunc, true);
		}
		else if(val == down_value)
		{
			result = true;
			// Value to delete the child
			children[i] = NULL;
		}
		else if(val == up_value)
		{
			// Propagate the value
			dummyValue = up_value;
			for(int j = 0; j < i; j++)
			{
				if(children[j] != DUMMY_LEAF)
				{
					if(negate)
						assignments.del(children[j]->negFunc, true);
					else
						assignments.del(children[j]->thisFunc, true);
				}
			}
			return true;
		}
		else
		{
			assert(false); // There are no other valid options!
		}
	}

	
	fixLeafs();

	for(i = 0; i < children.size(); i++)
	{
		result |= children[i]->checkDown(assignments);
	}

	for(i = 0; i < leafs.size(); i++)
	{
		assert(leafs[i] != DUMMY_LEAF);
		assignments.del(leafs[i] * multiple);
	}

	for(i = 0; i < children.size(); i++)
	{
		assert(children[i] != NULL);
		if(negate)
			assignments.del(children[i]->negFunc, true);
		else
			assignments.del(children[i]->thisFunc, true);
	}

	if(empty())
		dummyValue = down_value;

	return result;
}

int DummyTreeNode::fixTree()
{
	if(dummyValue != VALUE_UNASSIGNED)
		return dummyValue;

	if(type == QA_GATE || type == QE_GATE)
		return children[0]->fixTree();
	if(type == NOT_GATE)
	{
		int value = children[0]->fixTree();
		return (value == VALUE_UNASSIGNED? value : opposite(value));
	}

	assert(type == AND_GATE || type == OR_GATE);

	for(int i = 0; i < children.size(); i++)
	{
		int value = children[i]->fixTree();
		if(value == up_value)
			return up_value;
		else if(value == down_value)
		{
			children[i]->wipeOut();
			delete(children[i]);
			children[i]=children[children.size()-1];
			children.pop_back();
			i--;
		}
	}

	if(empty())
	{
		dummyValue = down_value;
		return dummyValue;
	}

	return VALUE_UNASSIGNED;

}


// return true if n1 and n2 are the same (or negations, if negate is true)
// Assumes thisFunc of children are initialized for both
// Works only for AND and OR gates
// Assumes children have been uniquely sorted
//   and have no repeats
bool sameFunc(DummyTreeNode * n1, DummyTreeNode* n2, bool negate)
{
	int i;

	if(n1->type != AND_GATE && n1->type != OR_GATE)
		return false;
	if(n2->type != AND_GATE && n2->type != OR_GATE)
		return false;
	if(n1->leafs.size() != n2->leafs.size())
		return false;
	if(n1->children.size() != n2->children.size())
		return false;

	if((negate && (n1->type == n2->type))
		|| (!negate && (n1->type != n2->type)))
		return false;
	
	// Check leafs
	int mult = 1;
	if(negate)
		mult = -1;
	for(i = 0; i < n1->leafs.size(); i++)
	{
		if(n1->leafs[i] != (n2->leafs[i] * mult))
			return false;
	}

	// Check children
	for(i = 0; i < n1->children.size(); i++)
	{
		if(!negate && n1->children[i]->thisFunc != n2->children[i]->thisFunc)
			return false;

		// Want at least one negation.
		// Since we assume thisFunc is initialized, it can't be zero.
		// So, equality means that functions are negations
		if(negate && (n1->children[i]->negFunc != n2->children[i]->thisFunc)
			  && (n1->children[i]->thisFunc != n2->children[i]->negFunc))
			return false;
	}

	// All checks passed
	return true;
}

// Find among the candidates the one that matches curr (negated if 'negate')
// Return the function number, or -1 if none found
DummyTreeNode *findFunc(vector<DummyTreeNode *> &candidates, DummyTreeNode *curr, bool negate)
{
	for(int i = 0; i < candidates.size(); i++)
	{
		if(sameFunc(candidates[i], curr, negate))
			return candidates[i];
	}
	return false;
}

void DummyTreeNode::nameFunctions(map<long, vector<DummyTreeNode*> > &hashMap, vector<DummyTreeNode *> &functions, int vars)
{
	Common::checkTime();

	// We assume that the tree was fixed.
	// In that case, we should never encounter constants
	assert(dummyValue == VALUE_UNASSIGNED);

	thisFunc = 0;
	negFunc = 0;

	int i;
	// Do the recursive call: want children initialized
	qGates = 0;

	for(i = 0; i < children.size(); i++)
	{
		children[i]->nameFunctions(hashMap, functions,vars);
		qGates += children[i]->qGates;
	}

	if(type == QA_GATE || type == QE_GATE)
	{
		qGates++;
	}

	if(qGates > 0)
		return; // We don't deal with sub-quantified things...

	sortChildren();

	// Is this function already there?
	long hashKey = hashValue(false, vars);	
	vector<DummyTreeNode *> &funcList = hashMap[hashKey];
	DummyTreeNode *sameNode = findFunc(funcList, this, false);

	
	if(sameNode == NULL)
	{
		// Didn't find a function: want to create one
		thisFunc = functions.size() + vars;
		functions.push_back(this);
		funcList.push_back(this);
		sameNode = this;
	}
	else
		thisFunc = sameNode->thisFunc;

	if(sameNode->negFunc != 0)
		negFunc = sameNode->negFunc;
	else
	{
		// Want to find negations, if exist
		hashKey = hashValue(true, vars);	
		vector<DummyTreeNode *> &funcList = hashMap[hashKey];
		DummyTreeNode *negNode = findFunc(funcList, this, true);

		if(negNode != NULL)
		{
			// The other node's negation should've been negNode.
			// If it isn't, it means it's not initialized yet,
			// which can only happen if we got a new function
			assert(sameNode == this);

			// Set mutual negation relationship
			negNode->negFunc = thisFunc;
			negFunc = negNode->thisFunc;
		}
	}
}

long DummyTreeNode::hashValue(bool negate, int vars)
{
	assert(type == AND_GATE || type == OR_GATE);
	long hashValue = 1;
	for(int i = 0; i < children.size(); i++)
	{
		hashValue *= 1021;
		if(negate)
			hashValue += children[i]->negFunc * 2 + vars;
		else
			hashValue += children[i]->thisFunc * 2 + vars;
		hashValue %= 104729;
	}

	for(int i = 0; i < leafs.size(); i++)
	{
		hashValue *= 1021;
		if(negate)
			hashValue += vars - leafs[i];
		else
			hashValue += leafs[i] + vars;
		hashValue %= 104729;
	}

	hashValue *= 2;
	if((type == AND_GATE) == negate)
		hashValue++;
	// (+0 otherwise)

	return hashValue;
}


int dummyTreePreproc(DummyTreeNode* node, int vars, int *allVars)
{
	vars++; //Want it to be more than the last var.
	int retval = VALUE_UNASSIGNED;
	
	map<long, vector<DummyTreeNode*> > hashMap;
	vector<DummyTreeNode *> functions;
	node->nameFunctions(hashMap, functions, vars);

	IntSet assignments(vars + functions.size());
	while(node->checkDown(assignments))
	{
		Common::checkTime();
		retval = node->fixTree();
		if(retval != VALUE_UNASSIGNED)
			return retval;

		hashMap.clear();
		functions.clear();

		node->nameFunctions(hashMap, functions, vars);
		assignments.clear(vars+functions.size());
	}

	*allVars = vars+functions.size();	
	return retval;
}


