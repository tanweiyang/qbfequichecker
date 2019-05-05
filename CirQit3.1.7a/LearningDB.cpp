#include "LearningDB.h"
#include "Solver.h"

// Lists that are used
#define TRAIL_LIST 0
#define BLOCK_LIST 1
#define LISTS_NUM 2

// This can be the same as one of the previous lists, since for 
// the list of empty nodes nothing else is used
// In fact, function clear() counts on EMPTY_LIST being one of the existing lists
#define EMPTY_LIST 0

// The initial capacity of the OrderedNodeList
#define C_INIT 20
// The increment of the OrderedNodeList
#define C_INCR 10

extern bool verbose; //(*) Think about how to do it better

class ListNode
{
public:
	Literal *lit;
	ListNode *next[LISTS_NUM];
	ListNode *prev[LISTS_NUM];

	ListNode(Literal *l) : lit(l)
	{};
};

void printTmpID();


class OrderedNodeList
{
private:
	// There are LISTS_NUM lists, each with list pointer, count and capacity.
	// list[i][index] is a pointer to the head of a doubly linked list with 
	//        elements whose value (of type i) == index
	ListNode **list[LISTS_NUM];
	int count[LISTS_NUM];
	int capacity[LISTS_NUM];
	int nodes;

	short linetype;

	ListNode *empties;

private:

	int getValue(int value_type, DagNode *node)
	{
		if(value_type == TRAIL_LIST)
		{
			int level = node->getDLevel(linetype);
			if(level == -1)
			{
				//assert(node->isUniversal()); //(*) Remove if might get unassigned existentials
				level = 0;
			}
			return level;
		}
		else if(value_type == BLOCK_LIST)
			return node->getBlockNum();
		else
		{
			assert(false);
			return -1;
		}
	}

	int getValue(int value_type, Literal *l)
	{
		return getValue(value_type, l->variable);
	}

	inline void empty(ListNode *node)
	{
		node->lit = NULL;
		node->next[EMPTY_LIST] = empties;
		empties = node;
	}

	inline ListNode *getListNode(Literal *l)
	{
		ListNode *result;

		if(empties != NULL)
		{
			result = empties;
			result->lit = l;
			empties = empties->next[EMPTY_LIST];
		}
		else
			result = new ListNode(l);

		return result;
	}

	void remove(ListNode *l)
	{
		nodes--;
		for(int i = 0; i < LISTS_NUM; i++)
		{
			if(l->next[i] != NULL)
				l->next[i]->prev[i] = l->prev[i];

			if(l->prev[i] != NULL)
				l->prev[i]->next[i] = l->next[i];
			else
			{
				list[i][getValue(i, l->lit)] = l->next[i];
			}
		}
		l->next[EMPTY_LIST] = NULL;
		l->prev[EMPTY_LIST] = NULL;
		empty(l);
	}

	inline ListNode *peekDeepest(int type)
	{
		assert(count[type] > 0);
		ListNode *result = list[type][count[type] - 1];
		assert(result != NULL);
		if(result->lit->variable->getReason(linetype) == NULL) // This is a decision variable
		{
			if(result->next[type] != NULL)
				result = result->next[type]; // Prefer non-decision variables
			//(*) Prefer deeper variables, even within the same block?
		}
		return result;
	}

	Literal *popDeepest(int type)
	{
		ListNode *l = peekDeepest(type);
		Literal *result = l->lit;

		remove(l);
		for(int i = 0; i < LISTS_NUM; i++)
		{
			while(count[i] > 0 && list[i][count[i] - 1] == NULL)
				count[i] --;
		}

		assert((count[type] == 0 && count[1 - type] == 0) || list[type][count[type] - 1] != NULL);
		return result;
	}

public:
	bool empty()
	{
		return nodes == 0;
	}

	int size()
	{
		return nodes;
	}

	OrderedNodeList()
	{
		for(int i = 0; i < LISTS_NUM; i++)
		{
			list[i] = (ListNode **)mallocC(sizeof(ListNode *) * C_INIT);
			capacity[i] = C_INIT;
			count[i] = 0;
		}
		empties = NULL;
		nodes = 0;
	}

	~OrderedNodeList()
	{
		int listN = 0;
		// Since all lists contain the same elements, need to go through only one
		ListNode* curr;
		ListNode* next;

		for(int i = 0; i < count[listN]; i++)
		{
			curr = list[listN][i];

			while(curr != NULL)
			{
				next = curr->next[listN];
				delete(curr);
				curr = next;
			}
		}

		curr = empties;
		while(curr!=NULL)
		{
			next = curr->next[EMPTY_LIST];
			delete(curr);
			curr = next;
		}

		for(int i = 0; i < LISTS_NUM; i++)
		{
			free(list[i]);
		}
	}

	void add(Literal *l)
	{
		assert(linetype == TYPE_1LINE || linetype == TYPE_0LINE);

		nodes++;
		ListNode *node = getListNode(l);
		for(int i = 0; i < LISTS_NUM; i++)
		{
			int value = getValue(i, l);

			// If necessary, increase the size of the indexing array
			if (value >= capacity[i])
			{
				capacity[i] += value + C_INCR;
				list[i] = (ListNode**)realloc(list[i], sizeof(ListNode *) * capacity[i]);
				assert(list[i] != NULL); // Out of memory
			}

			// If necessary, increase the active part of the indexing array
			while(value >= count[i])
			{
				list[i][count[i]] = NULL;
				count[i]++;
			}
			
			ListNode *prev = NULL;
			ListNode *curr = list[i][value];
			int t_level = l->variable->getTLevel(linetype);
			if(i == TRAIL_LIST)
			{
				// Make sure that variables are in trail order within the decision level
				while(curr != NULL && curr->lit->variable->getTLevel(linetype) > t_level)
				{
					prev = curr;
					curr = curr->next[i];
				}
			}

			node->next[i] = curr;
			if(prev == NULL)
				list[i][value] = node;
			else
				prev->next[i] = node;

			node->prev[i] = prev;
			if(curr != NULL)
				curr->prev[i] = node;
		}
	}

	// Linetype is 0LINE or 1LINE
	void clear(int linetype_n)
	{
		linetype = linetype_n;

		nodes = 0;
		for(int j = 0; j < count[EMPTY_LIST]; j++)
		{
			if(list[EMPTY_LIST][j] != NULL)
				empty(list[EMPTY_LIST][j]);
		}

		for(int i = 0; i < LISTS_NUM; i++)
		{	
			count[i] = 0;
		}
	}

	Literal *popTrailDeepest()
	{
		return popDeepest(TRAIL_LIST);
	}

	Literal *peekTrailDeepest()
	{
		return peekDeepest(TRAIL_LIST)->lit;
	}

	Literal *popBlockDeepest()
	{
		return popDeepest(BLOCK_LIST);
	}

	Literal *peekBlockDeepest()
	{
		return peekDeepest(BLOCK_LIST)->lit;
	}

	Literal *contains(Literal *l)
	{
		assert(l->value!=VALUE_DONT_CARE);
		return contains(l->variable, l->value);
	}

	// Does this set contain a literal with the same variable as given?
	// If so, return that literal
	// Search by block number, since trail depth might change
	Literal * contains(DagNode *node, int valueM = VALUE_DONT_CARE)
	{
		int value = getValue(BLOCK_LIST, node);
		if(value >= count[BLOCK_LIST])
			return false;
		ListNode *curr = list[BLOCK_LIST][value];
		while(curr != NULL)
		{
			if(curr->lit->variable == node)
			{
				return curr->lit;
			}
			curr = curr->next[BLOCK_LIST];
		}
		return NULL;
	}

	bool hasImplicant()
	{
		return list[TRAIL_LIST][count[TRAIL_LIST] - 1]->next[TRAIL_LIST] == NULL;
	}

	// Return second last trail level
	int lastBTLevel()
	{
		int i = count[TRAIL_LIST] - 2;
		while(i > 0 && list[TRAIL_LIST][i] == NULL)
			i--;
		return i;
	}

	// Fill the provided arrays with literals, in the order of block_num
	// Assumes arrays are created and of the right size
	// existentials and universals are for the current linetype
	void exportPointers(Literal** existentials, Literal** universals)
	{
		int cu=0, ce=0;
		for(int i = 0; i < count[BLOCK_LIST]; i++)
		{
			ListNode *curr = list[BLOCK_LIST][i];
			while(curr != NULL)
			{
				if(curr->lit->variable->isUniversal(linetype))
				{
					universals[cu] = curr->lit;
					cu++;
				}
				else
				{
					existentials[ce] = curr->lit;
					ce++;
				}
				curr = curr->next[BLOCK_LIST];
			}
		}
	}

	void print()
	{
		if(verbose)
			cout << "(size = " << nodes << ")\n";

		int type = BLOCK_LIST;
		for(int i = 0; i < count[type]; i++)
		{
			ListNode *curr = list[type][i];
			while(curr != NULL)
			{
				if(verbose)
				{
					cout << curr->lit->variable->getName() << "(" << valueToString( curr->lit->value) << ", ";
					cout << curr->lit->variable->getDLevel(linetype) << " ), ";
				}
				curr = curr->next[BLOCK_LIST];
			}
		}
	}
};


ComReason::ComReason()
{
	active = new OrderedNodeList();
}

ComReason::~ComReason()
{
	delete(active);
}


void ComReason::clear(int linetype_n, Reason* conflict_clause)
{
	linetype = linetype_n;
	active->clear(linetype_n);
	universals=existentials=0;
	firstUnset = -1;
	unsetCount = 0;

        if(Common::do_proof)
		Common::proof_file << linetype;
	addReason(conflict_clause, NULL, -1);
}

//(*) Everywhere a vector is used, initialize and resize to a greater amount

void ComReason::add(Literal *lit)
{
	Literal *act = active->contains(lit);

	bool needToAdd = true;

	if(act != NULL)
	{
		assert(act->value == lit->value);
		needToAdd = false;
	}

	if(needToAdd)
	{
		active->add(lit);

		if(!lit->variable->isAssigned(linetype))
		{
			int block = lit->variable->getBlockNum();
			if(unsetCount == 0 || block < firstUnset)
			{
				unsetCount = 1;
				firstUnset = block;
				if(verbose)
					cout << "                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ new unsets (" << firstUnset << ") " << unsetCount << "\n";
			}
			else if(block == firstUnset)
			{
				unsetCount++;
				if(verbose)
						cout << "                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ +++ unsets (" << firstUnset << ") " << unsetCount << "\n";
			}
		}

		if(lit->variable->isUniversal(linetype))
		{
			universals++;
		}
		else
		{
			existentials++;
		}
	}

	if(verbose)
		cout << "\tAdded " << lit->variable->getName() << " = \t" << valueToString( lit->value ) << " ("  << lit->variable->getDLevel(linetype) << ")\tto ComReason\n";
}


inline bool beReduced(Literal *lit, short linetype)
{
	return lit->variable->isUniversal(linetype);
}

void ComReason::removeDeepestLit(bool block, Literal *lit)
{
	Literal *lit2;
	if(block)
		lit2 = active->popBlockDeepest();
	else
		lit2 = active->popTrailDeepest();

	if(lit->variable->isUniversal(linetype))
		universals--;
	else
		existentials--;

	assert(lit2 == lit);

	if(!lit->isTrue(linetype) && lit->variable->getBlockNum() == firstUnset)
	{
		unsetCount--;
		if(verbose)
			cout << "                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ --- unsets (" << firstUnset << ") " << unsetCount << "\n";
		assert(unsetCount >=0); // We can't remove what we don't have
	}
}

// (*) This should be the only function to remove literals
// Attempt to remove the deepest literal in this reason
// If block is true, attempt at the block-deepest.
// Otherwise, attempt at the trail-deepest
bool ComReason::removeDeepest(bool block)
{
	Literal *lit;
	if(block)
		lit = active->peekBlockDeepest();
	else
		lit = active->peekTrailDeepest();

	if(beReduced(lit, linetype))
	{
		if(active->peekBlockDeepest()->variable->getBlockNum() > lit->variable->getBlockNum())
		{
			assert(block == false);
			return false;
		}
		else
		{
			if(verbose)
				cout << "Reduced " << lit->variable->getName() << " (block " << block << ")\n";
			if(Common::do_proof)
			{
				Common::proof_file << " u ";
				lit->proofPrint();
			}
			removeDeepestLit(block, lit);
			return true;
		}
	}
	else
	{
		// This cannot fail
		removeDeepestLit(block, lit);
		replaceLit(lit);

		// Do the universal reduction
		while(active->size() > 0 && beReduced(active->peekBlockDeepest(), linetype))
		{
			removeDeepest(true);
		}

		return true;
	}
}

void ComReason::addReason(Reason *r, DagNode* implied, int depth = -1)
{
	if(Common::do_proof)
	{
		Common::proof_file  << " ( ";
		r->printProofClause();
	}
	assert(r != NULL);

	int reduceTo = depth;
	int end = r->getSize();
	bool doneRes = false;

	// Check for any true literals. They will have to be reduced away
	// Before adding the reason
	if(implied != NULL) // allow those in initial conflict... Should we?
	{
		for(int i = 0; i < end; i++)
		{
			Literal *lit = r->litAt(i);
			if(lit->variable == implied)
				continue;
			if(!lit->isTrue(linetype)) // We are looking at opposites, so this will become true in clause
			{
				assert(beReduced(lit, linetype));
				int block = lit->variable->getBlockNum();
				if(reduceTo == -1 || block < reduceTo)
					reduceTo = block;
			}
		}
	}

	// Do the actual insertions.
	for(int i = 0; i < end; i++)
	{
		Literal *lit = r->litAt(i);
		if(lit->variable == implied)
		{
			if(Common::do_proof)
			{
				Common::proof_file << " a ";
				lit->proofPrint();
			}
			continue;
		}
		if(reduceTo < 0 || lit->variable->getBlockNum() < reduceTo)
		{
			// This literal is safe to add
			if(Common::do_proof)
			{
				Common::proof_file << " a ";
				lit->proofPrint();
			}
			add(lit);
		}
		else
		{
			if(beReduced(lit,linetype))
			{
				doneRes = true;
				 // Don't add: all prefix-deeper will be resolved
				if(Common::do_proof)
				{
					Common::proof_file << " l ";
					lit->proofPrint();
				}
			}
			// Note: lit was assigned before some universal u with u <=_q lit
			// So, lit may not have been a decision variable, and so, has a reason
			else
			{
				doneRes = true;
				if(Common::do_proof)
				{
					Common::proof_file << " r ";
					lit->proofPrint();
				}
				addReason(lit->variable->getReason(linetype), lit->variable, reduceTo);
			}
			// Note: there will not be invalid reductions.
			// If the recursive call increased reduceTo, then some u in res(lit) is
			// true. Then lit <_p u, and omitting lit is not a problem.
			// Otherwise, consider doing all the resolutions, and then replacing lit
			// with its reason in the current reason. Here, all existentials after reduceTo
			// have also been eliminated, so the universals can be reduced.
		}
	}
	if(doneRes)
	{
		if(Common::do_proof)
		{
			printTmpID();
		}
	}
	if(Common::do_proof)
		Common::proof_file << " ) ";
}

// Resolve the comReason with this literal
void ComReason::replaceLit(Literal *lit)
{
	if(verbose)
		cout << "Expanding " << lit->variable->getName() << "\n";

	// replace lit
	Reason *r = lit->variable->getReason(linetype);

	if(Common::do_proof)
	{
		Common::proof_file << " r ";
		lit->proofPrint();
	}

	assert((r != NULL && r->getSize() > 0) || lit->variable->getDLevel(linetype) <= 1);
	addReason(r, lit->variable, -1);

	if(verbose)
	{
		cout << "---\n";
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		print();
		cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	}

}

void ComReason::exportPointers(Literal*** replaceList, Literal*** reduceList)
{
	assert(existentials + universals == active->size());
	(*replaceList) = (*reduceList) = NULL;
	if(existentials > 0)
		(*replaceList) = (Literal**)mallocC(sizeof(Literal*) * existentials);

	if(universals > 0)
		(*reduceList) = (Literal**)mallocC(sizeof(Literal*) * universals);

	active->exportPointers(*replaceList, *reduceList);
}

bool ComReason::hasImplicant()
{
	// Multiple variables on the same level
	if(!active->hasImplicant())
		return false;

	DagNode *implicant = active->peekTrailDeepest()->variable;
	if(implicant->isUniversal(linetype))
	{
		// Wrong quantifier: need to continue expanding
		return false;
	}

	if(unsetCount > 0 && implicant->getBlockNum() > firstUnset)
		return false; // Implicant doesn't scope all unset variables

	// All checks passed
	return true;
}

Literal * ComReason::getImplicant()
{
	// (*) need to ensure (for conflicts) the existential implicant scopes all unassigned universals
	while(active->size() > 0 && !hasImplicant())
	{
		if(!removeDeepest(false)) // If we can't remove trail-deepest
		{
			bool result = removeDeepest(true);
			assert(result); // we remove quantifier-deepest
		}
	}

	if(active->size() > 0)
		return active->peekTrailDeepest();
	else
		return NULL;
}

void ComReason::getEmpty()
{
	while(active->size() > 0)
	{
		bool result = removeDeepest(true);
		assert(result); // we remove quantifier-deepest
	}
	if(Common::do_proof)
	{
		printTmpID();
		//Common::proof_file << " : 0 \n";
	}
}

int ComReason::lastBTLevel()
{
	return active->lastBTLevel();
}


void ComReason::print()
{
	return active->print();
}

bool ComReason::empty()
{
	return active->empty();
}

bool ComReason::containsOnly(DagNode* node)
{
	return active->size()==1 && (active->contains(node) != NULL);
}


// Declration here because the only class that uses this is LearningDB
class LearnedSet : public Notifiable
{
public:
	static int freeId;
private:

	int id;
	int linetype;

	// The arrays of pointers to nodes, each array arranged by block number.
	Literal **myWatches; // Existentials for clauses, universals for cubes
	Literal **otherVars; // Variables of the type other than the watched type

	int numMy, numOther;

	Literal* watch1;
	Literal* watch2;
	Literal* falseW;

	LearningDB *db;

public:
	// Copies the values from ComReason, leaving it untouched
	LearnedSet(ComReason *set, int line_type, LearningDB *ldb);
	~LearnedSet();

	int notify(int value, short type, DagNode * watch, bool *remove );

	bool forceWatch(Circuit *c);
	int force(Circuit *c);

	short getLineType() { return linetype; };

	void print();

	int getClauseSize(DagNode* child);
	Literal *getClauseLiteral(DagNode* child, int index);
	void printProofClause(DagNode *child);

	void printProofIdAndLits()
	{
		Common::proof_file << " | " << id; 
/*
		Common::proof_file << " :";

		int i;
		for(i = 0; i < numMy; i++)
		{
			Common::proof_file << " ";
			myWatches[i]->proofPrint();
		}
		for(i = 0; i < numOther; i++)
		{
			Common::proof_file << " ";
			otherVars[i]->proofPrint();
		}
		Common::proof_file << " 0";
*/
		Common::proof_file << "\n";
	}
};

void printTmpID()
{
	Common::proof_file << " | " << LearnedSet::freeId;
	LearnedSet::freeId++;
}

int LearnedSet::freeId = 1;

bool LearnedSet::forceWatch(Circuit *c)
{
	Literal *implicant = watch1;
	if(watch1->variable->isAssigned(linetype))
	{
		if(watch2->variable->isAssigned(linetype))
		{
			// The watch was assigned after being triggered
			//if(watch2->variable->getValue() != watch2->value)
			//return false;

			//Since we're triggering immediately, this should not happen
			assert(false);
		}
		implicant = watch2;
	}

	c->beginNewReason(this, NULL);
/*	int i;
	for(i = 0; i < numMy; i++)
	{
		if(myWatches[i] != implicant)
			c->addToReason(myWatches[i]);
	}

	for(i = 0; i < numOther; i++)
	{
		c->addToReason(otherVars[i]);
	}
*/
	Signal value;
	value.value = opposite(implicant->value);
	value.type = linetype;
	value.justified = false;
	implicant->variable->addToUPStack(implicant->variable, 
		value, c->last_reason, true);

	if(c->verbose)
	{
		if(linetype == TYPE_0LINE)
			cout << "triggering forced literal from cube " << this << "(" << implicant->variable->getName() << ")" << endl;
		else
			cout << "triggering forced literal from no good " << this << "(" << implicant->variable->getName() << ")" << endl;
	}
	return true;
}

int LearnedSet::force(Circuit *c)
{
	int result;
	if(linetype == TYPE_1LINE)
		result = CONFLICT;
	else
		result = QSAT;

	c->resetLearnedSet(linetype, this, NULL);

	return result;
}

void LearnedSet::print()
{
	int i;

	cout << "MyWatches: ";
	for(i = 0; i < numMy; i++)
	{
		cout <<  myWatches[i]->variable->getName() << '=' << valueToString(myWatches[i]->value);
		cout << "(" << myWatches[i]->variable->getDLevel(linetype) << "), ";
	}
	cout << '\n';
	cout << "Other: ";
	for(i = 0; i < numOther; i++)
	{
		cout <<  otherVars[i]->variable->getName() << '=' << valueToString(otherVars[i]->value);
		cout << "(" << otherVars[i]->variable->getDLevel(linetype) << "), ";
	}

}


LearnedSet::LearnedSet(ComReason *set, int line_type, LearningDB *ldb)
{
	id = freeId++;

	db = ldb;
	linetype = line_type;
	assert(set->getLineType() == linetype);

	set->exportPointers(&myWatches, &otherVars);
	numMy = set->getExistentials();
	numOther = set->getUniversals();

	assert(numMy > 0);

	watch1 = NULL; // Should be unassigned watch, or, of no unassigned, the deepest
	watch2 = NULL;

	// Look for the two trail_deepest watches
	int trail_deepest = -1;

	for(int i = 0; i < numMy; i++)
	{
		// Want to get unassigned variables first
		if(myWatches[i]->variable->getDLevel(linetype) > trail_deepest || !myWatches[i]->variable->isAssigned(linetype))
		{
			int depth = myWatches[i]->variable->getDLevel(linetype);
			assert(depth > 0 || watch1 == NULL || watch1->variable->isAssigned(linetype)); // Assert at most 1 unassigned variable
			if(watch1 == NULL || depth==-1 || // have no watch, or the new one is unassigned
				(depth > watch1->variable->getDLevel(linetype) && watch1->variable->isAssigned(linetype))) // or this is deeper
									// Note: never replace the unassigned watch
			{
				watch2 = watch1;
				watch1 = myWatches[i];
			}
			else if(watch2 == NULL || depth > watch2->variable->getDLevel(linetype))
			{
				watch2 = myWatches[i];
				trail_deepest = depth;
			}
		}
		myWatches[i]->variable->incrementVSIDS(line_type);
	}

	assert(watch1 != NULL);
	watch1->variable->addNotify(watch1->value, linetype,  this);

	for(int i = 0; i < numOther; i++)
	{
		// There are no other myWatches. 
		// Need to find a scoping var of opposite type
		if(watch2 == NULL && otherVars[i]->variable->getDLevel(linetype) > trail_deepest 
				&& otherVars[i]->variable->getBlockNum() 
						< watch1->variable->getBlockNum())
		{
			trail_deepest = otherVars[i]->variable->getDLevel(linetype);
			watch2 = otherVars[i];
		}

		otherVars[i]->variable->incrementVSIDS(line_type);
	}

	if(watch2 != NULL)
		watch2->variable->addNotify(watch2->value, linetype, this);
	// else: no other myVars, and no scoping otherVars
	//       this means that unitImplicant will get this value at root level
	//       and this will never be undone. Then we don't need watches!

	falseW=NULL;
}

int LearnedSet::getClauseSize(DagNode* child)
{
	return numMy + numOther;
}

Literal *LearnedSet::getClauseLiteral(DagNode* child, int index)
{
	if(index < numMy)
		return myWatches[index];
	return otherVars[index - numMy];
}

void LearnedSet::printProofClause(DagNode *child)
{
	Common::proof_file << "c-" << id;
}

// (*) Delete inactive learnedSets

LearnedSet::~LearnedSet()
{
	delete myWatches;
	delete otherVars;
}

int LearnedSet::notify(int value, short type, DagNode * watch, bool *remove )
{
	assert(type == linetype);

	*remove = false;

	if(falseW!=NULL && (falseW->variable->isAssigned(linetype) || 
		falseW->variable->isJustified(linetype)) && !falseW->isTrue(linetype))
	{
		// This set won't work anyway - got a false literal. Don't need to do anything
		return OK;
	}

	int retval = OK;

	// find a new watch if one exists
	Literal * new_watch1 = NULL;
	Literal * new_watch2 = NULL;

	// Look for watches
	for( int i = numMy-1; i >= 0; i-- ) // Want to try trail-deepest watches first
	{
		if(!myWatches[i]->isTrue(linetype))
		{
			if(myWatches[i]->variable->isAssigned(linetype) || myWatches[i]->variable->isJustified(linetype))
			{
				//(*) test
				// Found a false literal - don't need to do anything else
				falseW = myWatches[i];
				return OK;
			}

			if(new_watch1 == NULL)
				new_watch1 = myWatches[i];
			else
			{
				new_watch2 = myWatches[i];
				break;
			}
		}
	}

	if(new_watch2 == NULL)
	{
		// Here we can infer something, unless some unset or true variables prevent it

		// Look for problems from otherVars, find the outermost scoping unset variable

		Literal *scoper = NULL;
		// Are there any otherVars scoping by the set (single) watch?
		for(int i = 0; i < numOther; i++) // Look for first-block unassigned other
		{
			if(!otherVars[i]->isTrue(linetype))
			{
				if(otherVars[i]->variable->isAssigned(linetype) || otherVars[i]->variable->isJustified(linetype))
				{
					//(*) test
					// Found a false literal - don't need to do anything else
					falseW = otherVars[i];
					return OK;
				}

				if(scoper == NULL || otherVars[i]->variable->getBlockNum() < scoper->variable->getBlockNum())
				{
					// Found a better scoping variable.
					scoper = otherVars[i];
				}
			}
		}

		if(new_watch1 == NULL)
		{
			// All (my type) variables are true. The rest get reduced
			// This constraint is triggered and causes a conflict/solution
			retval = db->trigger(this);
		}
		else
		{
			if(scoper == NULL || scoper->variable->getBlockNum() > new_watch1->variable->getBlockNum())
			{
				// No scoping variables -> they are all reduced.
				// Can force the new_watch1 to the needed value
				if(new_watch1 != watch1 && new_watch1 != watch2)
				{
					// This could only happen if watch1 changed value,
					// and it was watched by a universal
					Literal **rediWatch = &watch2;
					if(beReduced(watch1, linetype))
						rediWatch = &watch1;
					assert(beReduced((*rediWatch), linetype));
					assert((*rediWatch)->variable != watch);
					(*rediWatch)->variable->removeNotify((*rediWatch)->value, linetype, this);
					new_watch1->variable->addNotify(new_watch1->value, linetype, this);
					(*rediWatch) = new_watch1;
				}
				assert(!new_watch1->variable->isAssigned(linetype));
				retval = db->trigger(this, new_watch1);
			}
			else
			{
				// There is a scoping variable that prevents forcing of new_watch1.
				// So, take it as a watch
				new_watch2 = scoper;
			}
		}
	}

	if(retval == OK && new_watch2 != NULL)
	{
		// Set the watches
		if(new_watch1 == watch2 || new_watch2 == watch1) // switch w1 and w2
		{
			Literal *tmp = new_watch1;
			new_watch1 = new_watch2;
			new_watch2 = tmp;
		}

		if(new_watch1 != watch1)
		{
			if(watch1->variable != watch)
				watch1->variable->removeNotify(watch1->value, linetype, this);
			else
				*remove = true;

			new_watch1->variable->addNotify(new_watch1->value, linetype, this);
			watch1 = new_watch1;
		}

		if(new_watch2 != watch2)
		{
			if(watch2->variable != watch)
				watch2->variable->removeNotify(watch2->value, linetype, this);
			else
				*remove = true;

			new_watch2->variable->addNotify(new_watch2->value, linetype, this);
			watch2 = new_watch2;
		}
	}

	return retval;

}

LearningDB::LearningDB(Circuit *c)
{
	circuit = c;
	for(int i = 0; i < VAL_TYPES; i++)
		count[i] = 0;
}

LearningDB::~LearningDB()
{
	for(int type = 0; type<VAL_TYPES; type++)
	{
		for(int i = 0; i < stored[type].size(); i++)
		{
			delete(stored[type][i]);
		}
		count[type] = 0;
	}
}


void LearningDB::addSet(ComReason *set, int linetype, bool force)
{
	LearnedSet *newSet = new LearnedSet( set, linetype, this);
	stored[linetype].push_back(newSet);
	count[linetype]++;

	if( circuit->verbose)
	{
		cout << "Adding Learned Set to db: ";
		cout << newSet << ": ";
		newSet->print();
		cout << "\n\n";
	}
	if( Common::do_proof )
	{
		newSet->printProofIdAndLits();
	}
	if( circuit->verbose && count[linetype] % 100 == 0 ) 
		cout << "No Goods: " << count[linetype] << endl;
	if( count[linetype] % 5000 == 0 ) circuit->decayVSIDS(linetype);

	if(force)
		newSet->forceWatch(circuit);
	//(*) prune db (*)
}


void LearningDB::resetDB()
{
	while( !true_sets.empty() ) 
		true_sets.pop();
	while( !prop_sets.empty() )
		prop_sets.pop();
}

int LearningDB::checkStatus()
{
	int retval = OK;
	while( !true_sets.empty() )
	{
		LearnedSet * top = true_sets.top();
		true_sets.pop();
		retval = top->force(circuit);
		if(retval != OK)
			break;
	}

	// The rest of true_sets will be popped in resetDB;
	return retval;
}

int LearningDB::propagateDB()
{
	int retval = checkStatus();
	if( retval != OK ) return retval;

	while( !prop_sets.empty() )
	{
		LearnedSet * top = prop_sets.top();
		prop_sets.pop();

		if(top->forceWatch(circuit))
		{
			forced_count[top->getLineType()]++;
			break;
		}
	}

	return retval;
}

int LearningDB::trigger(LearnedSet *l)
{
#ifdef DB_IMMEDIATE
	return l->force(circuit);
#else
	true_sets.push(l);
	return OK;
#endif
}

int LearningDB::trigger(LearnedSet *l, Literal* implicant)
{
#ifdef DB_IMMEDIATE
	if(l->forceWatch(circuit))
	{
		forced_count[l->getLineType()]++;
	}
	return OK; // Only add to the up_stack
#else
	prop_sets.push(l);
	return OK;
#endif
}
