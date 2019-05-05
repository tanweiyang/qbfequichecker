#ifndef _cCIRCUIT_H_
#define _cCIRCUIT_H_

#include "Common.h"
#include "DagNode.h"
#include <vector>
#include <map>
#include <stack>
#include "NodeFactory.h"
#include "PropRecord.h"
#include "UndoRecord.h"
#include "Literal.h"
#include "Allocator.h"
#include "LearningDB.h"
#include "LeafNode.h"


#define EXISTENTIAL	0x10
#define UNIVERSAL	0x11


typedef map<string,DagNode*> VariableMap;

struct QuantifierBlock {
    int type;
    DagNode * * variables;
    int num_vars;
};


class Circuit
{

private:

	// ------------------Needed for set-up only:
	vector<string> outputs;

	// Add a new variable with the given name with the given pointer
	// Add the new association: name-variable
	// Assumes there is no variable with the current name
	inline void storeVariable( string &name, DagNode *var)
	{
		// Variable map will make its own copy of the string
		variable_map.insert(make_pair(name, var));
	}

	// Add a new leaf variable with the given name;
	// If one already exists, return its location
	DagNode *addInput(string &var_name);

	// 'list' contains the list of variable names.
	// Find those variable nodes, or create if they don't already exist.
	// set their type and block_count to the given values; 
	// Add the pointers to those nodes to new_list, starting at index i.
	inline void addList( DagNode ** new_list, vector<string> &list, int type, int block_count, int i)
	{
		for(int j = 0; j < list.size(); j++)
		{
			new_list[i] = addInput( list[j] );
			new_list[i]->setQuantifier(type, block_count);
			i++;
		}
	}

	// Build the Dag by replacing children's names by pointers to nodes
	// For all the descendants of curr, recursively
	DagNode *buildDag(DagNode *curr);

	
	vector<QuantifierBlock *> quantifiers;
	VariableMap variable_map;


	//------------------------------------------
	// Members
	DagNode *root_node;
	vector<LeafNode *> inputs;

public:

	// Return the pointer to a node with the variable with the given name
	// If no such variable has been created, return NULL
	inline DagNode * lookupVariable(string name)
	{
		VariableMap::iterator iter;
		iter = variable_map.find( name );
		if( iter != variable_map.end() ) {
			// the variable already exists
			return iter->second;
		}
		return NULL;
	}

	//(*) Comment TODO
	void addQuantifier(vector<string> &list, int type);
	void addInputs(vector<string> &list);

	
	// Add the given name to the list of output variables
	void addOutput(string name);

	void addOutputs(vector<string> &list);
	void addGate(string name, int type, vector<string> &list);
	DagNode *getRoot_node(){return root_node;};

	// After all the add methods have been called, call this method
	// This method finalizes the circuit, creates the final links,
	// simplifies and flattens the gates
	// It also destroys all the data needed only for initialization
	void finalize();

	// For converting to QBF1.0
	void printQBF(DagNode *curr = NULL);

	/**************************************************************************
	Preprocessing:
	**************************************************************************/
	// Do preprocessing: merge nodes representing the same subformulas,
	// remove duplicates
	// Possibly other preprocessing
	// Returns true iff preprocessing was successful
	// False on error
	bool preprocess();

	// Go through the DAG starting at curr and do simplifications
	// Return true if something was changed, false otherwise
	bool simplify(DagNode *curr);

	// Go through the DAG and when necessary create 
	// new sub-gates for different quantifier levels
	//   -> This is done for better learned clauses
	void larify(DagNode *curr);


	// Get the number of quantifiers in this circuit
	inline int numQuantifiers() {return quantifiers.size(); };


	inline QuantifierBlock * getQuantifier( int curr_block ) {return quantifiers.at(curr_block); };

private:

	// --------------------
	// REMOVING DUPLICATE SUBFORMULAS:
	// --------------------
	map<int,NodeList*> hash_map;
	map<DagNode*,char> visited;

	// Get an int value related to this node's
	// type and children
	int getHashValue( DagNode * node );
	// Get hash value of NOT(node)
	int getHashValueNot( DagNode * node );
	int getHashValueNeg( DagNode * node );

	// Remove duplicate 
	void mergeCommonSubformulas(DagNode *);

	// Everywhere node b is mentioned, replace it by a
	void mergeNodes( DagNode * a, DagNode * b );
	
	// Remove duplicate pointers (pointers to the same node in memory)
	// From the child and parents lists of all nodes rooted at curr
	// skipping those that are mentioned in the 'visited' map
	void removeDupes(DagNode *curr);

public:
	// (*) insert description and add functionality (TODO)
	bool initializeAndVerify( DagNode * curr );

	// Check corrent number of parents for each child
	bool countParents(DagNode *curr, bool remove);

	// Print this circuit into the file in the ISCAS format
	void printIscas(ofstream &fout);
private:
	// Print the unvisited subtree rooted at curr into fout
	// in ISCAS format
	void printIscasCircuit( DagNode * curr, ofstream &fout );


	/**************************************************************************
	* Solving
	**************************************************************************/
public:
	void newDecisionLevel();
	void undoDecisionLevel();

	UPstack *up_stack;
	UndoStack *undo_stack;
	stack<DagNode *> *justified_stack;
	stack<short> *justified_types;
	bool verbose;

	void addJustified(DagNode *, short type);

	
	void decayVSIDS(int type);
	/****************************************************************************
	Concerning reasons and learning
	****************************************************************************/
private:
	// (*) comment TODO
	stack<int> reason_ptrs;
	int reason_ptr;

	// Allocators for reasons:
	Allocator * d_level_alloc; // Allocator for the current level
	stack<Allocator*> allocator_stack; // Allocators for all the levels

public:
	// Note: all interaction with this reason's internals should be
	// done through the solver functions below, 
	// to make sure nothing gets broken.
	Reason *last_reason;
	// Sets last_reason to a new reason. 
	// Note: the previous reason now can't be changed
	void beginNewReason(Notifiable *source, DagNode* child);
	void beginNewReason(int linetype, DagNode* root);


	void resetLearnedSet( int linetype, Notifiable *source, DagNode *child );
	void resetLearnedSet( int linetype, DagNode *root );
	void resetLearnedSet( int linetype, Reason* reason );

	ComReason *lastLearnedSet;
};



/******************************************************************************
Helper functions
******************************************************************************/
// Return true if the given nodes have the same type and children
// (thus, represent the same subformula)
bool compareNodes( DagNode * a, DagNode * b );


#endif
