#ifndef _dDAGNODE_H_
#define _dDAGNODE_H_

#include "Common.h"
#include<vector>
#include<string>
#include "Reason.h"
#include "Notifiable.h"

class Circuit;

#define VAL_TYPES 2
// 0LINE: signal in the dual circuit
#define TYPE_0LINE 0
// 1LINE: signal in the real circuit
#define TYPE_1LINE 1

#define TYPE_BOTH 5


struct assign_value
{
	short value;
	short type;
	bool justified;
};

typedef struct assign_value Signal;

// (*) TODO: comment all the definitions
#define GATE_TYPE       int
#define UNDEFINED       0
#define LEAF            1
#define AND_GATE        2
#define OR_GATE         3
#define NOT_GATE        4
// Gate for quantification:
#define QA_GATE          5
#define QE_GATE          6


#define EXISTENTIAL	0x10
#define UNIVERSAL	0x11


// Used for list of notify literals
typedef vector<DagNode*> NodeList;
typedef vector<Notifiable*> NotifyList;


// 2 right most bits for value
#define VALUE_MASK 0x03

class DagNode;
typedef vector<DagNode*> NodeList;

class DagNode : public Notifiable
{
public:
	vector<string> *childNames; // Used for initialization only

protected:
	// Current node's name
	char *name;

	// Children of this node
	DagNode ** children;
	// Number of children of this node
    int num_children;
	// Parents of this node
	NodeList * parents;
	
	struct
	{
		short value[VAL_TYPES];
		short phase; // Last value it was set to
		bool justified[VAL_TYPES];
		bool dont_care;
		bool universal;

		// If this variable gets a value, reason will be the clausal reason for that value
		Reason *reason[VAL_TYPES];
		// The level at which the current variable obtained the value
		int decision_level[VAL_TYPES];
		int trail_level[VAL_TYPES];
	};

	// various flags (see references)
	//char bit_data;


	// (*) Comment
	DagNode * justified_watch[VAL_TYPES];

protected:
	// The circuit this node is part of
	Circuit *circuit; //(*) make this const
	int type;
	int block_num;

public:
	~DagNode();
	DagNode(string &name, Circuit *c);
	/*************************************************************************
	Initalization only:
	*************************************************************************/
	// Create dummy nodes for children
	// with the given names
	void setDummyChildren(vector<string> &children);

	// Delete the previous child list, if any,
	// and set the list of children to the given list
	void setChildren( NodeList * list );

	char * getName() { return name; } ;
	// Set this node's quantifier to the given value
	void setQuantifier( int type, int block_num );

	// Sort children by quantifier level
	void sortChildren();

	// Set the current block number and quantifier based on children's values
	// For the root node, set its block number to 0
	// If single is true, assume the children are
	//       alredy correctly set, so no recursion
	void setBlockNum(bool root = false, bool single = false);

	// Returns the current node's block number
	int getBlockNum() {	return block_num; };
//(*) Move these? Reorganaze this header

	// Returns the reason for the current assignment, 
	//     NULL if unassigned or decision variable
	Reason *getReason(short type) {return reason[type]; };

	// Return the decision level of the current node,
	// -1 if unassigned
	int getDLevel(short type) { return decision_level[type];};
	int getTLevel(short type) { return trail_level[type];};

	// Add the given node to the list of this node's parents
	// (*) Think about representation of parents
	void addParent( DagNode * node );

	bool containsParent( DagNode * node );

	// Remove the given node from parents
	// This has to be called BEFORE initializing the watches!
	void removeParent( DagNode * node );

	// Return the number of parents of this node
	int getNumParents() { return parents->size(); };

	/*************************************************************************
	Accessor:
	*************************************************************************/
	// Return true iff this node's quantifier is universal
	bool isUniversal(short type);

	// If uni is true, set this node's quantifier to universal,
	// Otherwise, set to existential
	void setUniversal(bool uni);

	// Returns the type of this gate
	int getType() { return type; };

	// Get the list of children
	DagNode * * getChildren() { return children; };
	// Get the list of parents
	NodeList * getParents() {return parents; };

	// Get number of children
	int getNumChildren() { return num_children; };

	// Returns true iff this node is the leaf
	virtual bool isLeaf()
        {   return false;       };

	// Return this node's value or VALUE_UNASSIGNED
	int getValue(short type) {return value[type]; };

	int getPhase() {return phase; };

	// Return boolean representation of whether this node is justified
	// Justified means that the existing children are enough to support
	// this node's value, and all other children are don't care from
	// the point of view of this parent
	bool isJustified(short type);
	bool isAllJustified();


	// Return true iff this node has been assigned a value
	inline bool isAssigned(short type) {	return getValue(type) != VALUE_UNASSIGNED; };

	// The the current node's value to the diven one
	void setValue( short value, short type, int d_level, int trail_depth, Reason *r );

	inline void resetValues()
	{
		for(short i = 0; i < VAL_TYPES; i++)
		{
			value[i] = VALUE_TRUE; // For correct phase initialization inside setValue
			setValue(VALUE_UNASSIGNED, i, -1, -1, NULL);
			justified[i] = false;
		}
		dont_care = false;
	}

	// Reset the value to unassigned: used to undo the 
	// assignment on backtrack
	void resetValue(short type )
	{
		setValue( VALUE_UNASSIGNED, type, -1, -1, NULL );
		reason[type] = NULL;

		if(isLeaf())
		{
			short otherType = (type == TYPE_1LINE? TYPE_0LINE : TYPE_1LINE);
			setValue( VALUE_UNASSIGNED, otherType, -1, -1, NULL );
			reason[otherType] = NULL;
		}
	}

	/*************************************************************************
	Solving:
	*************************************************************************/

	// Give this node the given value
	// reason - the clausal reason for the assignment
	// ignore - will get populated with the value: 
	//		true if this change should not be propagated
	//		false if it should (made some change)
	// forced - true if this assignment was forced,
	//			false if it is a decision
	// d_level - the (trail) level at which the assignment took place
	int assign( Signal new_value, Reason * reason, bool& ignore, bool forced, int d_level, int d_size );

	// Add the given assignment to the stack
	void addToUPStack( DagNode * variable, Signal value, Reason * reason, bool forced );

	/*************************************************************************
	Notifiable and virtual functions:
	*************************************************************************/
	int notifyJustified( DagNode * node, short type, bool *remove);

	//(*) Add comments
	virtual int notify(int value, short type, DagNode *watch, bool *remove) = 0;
	virtual int propagate(short type) = 0;

	void setJustified(bool justified, short type, Reason *reason);

	//Force the value to be justified on both lines.
	//Does not store undo record.
	// Should only be used before initalization of the solver
	void forceJustified();

	// This is redefined in children, if they need to react
	virtual void incrementVSIDS(int type)
        {   /* do nothing */    };

	/*************************************************************************
	Watches:
	*************************************************************************/
private:
	// Nodes to notify when the following happens (at different indeces)
	//    VALUE_TRUE - this node becomes true
	//    VALUE_FALSE - this node becomes false
	//    VALUE
	NotifyList notify_list[NOTIFY_SIZE][VAL_TYPES];

public:
	// Add this node to the notify list for the given value
	void addNotify(int value, short type, Notifiable *node);

	// Remove the given node from the notify list
	// A node SHOULD NOT call this if it just received 
	// notification from this node, because that code
	// relies on the order of notify_list.
	// In that case, use the parameter *remove in the notify function. 
	void removeNotify(int value, short type, Notifiable* node);

	// Notify every node that should notify this node of true/false assignments
	virtual void initializeTrueFalseWatches() = 0;

	// Set the watches - assumes the DAG is built and finalized
	void initializeAllWatches();

	// Notify any nodes that depend on this one
	int notifyWatches(int index, short type);
	int notifyWatches(short type);

	// Make sure a parent notifies this when it becomes justified
	void initializeJustifiedWatches();

	// ----Unitility functions-------------------------------------------------
	// swap positions of two children
	// at the given indeces
	void swapChildren( int i, int j );
};

// Utils
const char * valueToString( int value );
int stringToValue(string s);


#endif
