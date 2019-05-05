#include "DagNode.h"
#include "Circuit.h"
#include <algorithm>


// Only this file needs these masks; hide from all others
// left most bits for flags
#define JUSTIFIED_FLAG_MASK 0x80    
#define WATCH_INIT_FLAG_MASK 0x40
#define UNIVERSAL_FLAG_MASK 0x20
#define UIP_REPLACE_FLAG_MASK 0x10
#define DC_FLAG_MASK 0x08


#define UNDEFINED_BLOCK -1

extern bool verbose; //(*) Think about how to do it better




DagNode::DagNode(string &n_name, Circuit *c)
{
	num_children = 0;
	children = NULL;
	universal = false;
	name = NULL;
	name = (char *) mallocC((n_name.length() + 1) * sizeof(char));
	strncpy( name, n_name.c_str(), n_name.length()+1 );
	childNames = NULL;
	block_num = UNDEFINED_BLOCK;
	parents = new NodeList();

	for(int t = 0; t < VAL_TYPES; t++)
	{
		reason[t] = NULL;
	}
	circuit = c;
	resetValues(); // Sets value's and d_level's and justified's
	//(*) TODO: make sure all relevant fields are initialized
}

DagNode::~DagNode()
{
	delete parents;
	delete name;
}


// utils

void DagNode::swapChildren( int i, int j )
{
    assert( i < num_children && j < num_children );
    DagNode * temp = children[i];
    children[i] = children[j];
    children[j] = temp;
}

// (*) make inline
const char * valueToString( int value )
{
    switch( value ) {
        case VALUE_UNASSIGNED:
            return "UNASSIGNED";
        case VALUE_TRUE:
            return "TRUE";
        case VALUE_FALSE:
            return "FALSE";
        case VALUE_DONT_CARE:
            return "DONT_CARE";
    }
    return "?";
}

int stringToValue(string s)
{
	if(s == "TRUE")
		return VALUE_TRUE;
	else if (s =="FALSE")
		return VALUE_FALSE;
	else
	{
		assert(false); // Invalid value - error message?(*)
		return VALUE_UNASSIGNED;
	}
}

// end utils


void DagNode::setDummyChildren(vector<string> &children)
{
	assert(childNames == NULL);
	childNames = new vector<string>;
	childNames->assign(children.begin(), children.end());
}


bool DagNode::isUniversal(short type)
{
	if(type == TYPE_1LINE)
		return universal;
	else
		return isLeaf() && !universal;
}

inline void DagNode::setUniversal( bool n_universal )
{
	universal = n_universal;
}

void DagNode::setQuantifier( int type, int n_block_num )
{
    block_num = n_block_num * 2;
    if( type == UNIVERSAL ) {
        setUniversal( true );
    } else {
        setUniversal( false );
    }
}


bool comp(DagNode *d1, DagNode *d2)
{
	return d1->getBlockNum() < d2->getBlockNum();
}

// Sort children[left ... right]
void DagNode::sortChildren()
{
	std::stable_sort(children, children+num_children);
}


void DagNode::setBlockNum(bool root, bool single)
{
	// (*) check how those block_nums behave
	int i;
	if(block_num != UNDEFINED_BLOCK)
		return; // Already set for this node

	assert(!isLeaf());

	int max_block = 0;

	for(i=0; i < num_children; i++)
	{
		if(!single)
			children[i]->setBlockNum();
		int block = children[i]->block_num;
		
		if(max_block < block)
			max_block = block;
	}

	if(root)
		block_num = 0;
	else
		block_num = max_block;

	if(block_num %2 == 0) // On leaf block
		block_num++;

	setUniversal(false);
	//setUniversal(allUniversal); 
	// (*) do we need a record of whether all children are existential? dynamically, maybe?
	// i.e. all unset children are existential/universal

	sortChildren();
}

void DagNode::setChildren( NodeList * list )
{
	if(children != NULL)
		delete children;
    children = new DagNode * [list->size()];
    NodeList::iterator i = list->begin();
    int j = 0;
    for( ; i != list->end(); i++, j++ ) {
        children[j] = *i;
		children[j]->addParent( this );
    }
    num_children = j;
    
    // because we copied it into the array, delete it
    delete(list);
}

void DagNode::addParent( DagNode * node )
{
	parents->push_back( node ); //(*) Check if an array can be used for parents, rather than vector TODO
}

bool DagNode::containsParent( DagNode * node )
{
	NodeList &pars = *parents;
	DagNode* p;
	
	for(int i = 0; i < pars.size(); i++)
	{
		p = pars[i];
		if(p == node)
			return true;
	}
	return false;
}

void DagNode::removeParent( DagNode * node )
{
	NodeList &pars = *parents;
	DagNode* p;
	
	for(int i = 0; i < pars.size(); )
	{
		p = pars[i];
		if(p == node)
		{
			pars[i] = pars[pars.size()-1];
			pars.pop_back();
		}
		else
			i++;
	}
}

// d_size is the trail depth of the literal
void DagNode::setValue( short n_value, short type, int d_level, int d_size, Reason *r )
{
	if(n_value == VALUE_UNASSIGNED)
		phase = value[type];

	value[type] = n_value;
    decision_level[type] = d_level;
	trail_level[type] = d_size;
	reason[type] = r;
}


int DagNode::assign( Signal new_value, Reason * reason, bool& ignore, bool forced, int d_level, int d_size )
{
	ignore = false;
	short type = new_value.type;

	assert(new_value.value != VALUE_DONT_CARE);
	assert(getValue(type) != VALUE_DONT_CARE);

	if( !isAssigned(type) )
	{
		if( isUniversal(type) && forced )
		{
			assert(!dont_care);
			// create the no good for this conflict (*)
			int result;

			if(type == TYPE_1LINE)
			{
				// conflict - forced universal
				if( verbose )   cout << "** Conflict - forced universal " << name << endl;

				result = CONFLICT;
			}
			else
			{
				assert(type == TYPE_0LINE);
				if( verbose )   cout << "** Solution - sufficient existential " << name << endl;

				result = QSAT;
			}

			if(reason == NULL)
			{
				assert(this == circuit->getRoot_node());
				circuit->resetLearnedSet(type, this);
			}
			else
				circuit->resetLearnedSet(type, reason );

			return result;
		}

		if(isJustified(type))
		{
			ignore = true;
			return OK; // Don't need to set the value
		}
		// (*)
		
	}
	else
	{
		// Already assigned
		if( getValue(type) == new_value.value )
		{
			// reassigning the same - ignore assignment
			if(new_value.justified)
				setJustified(true, type, reason); // Justify will set its own undo record if needed
			ignore = true;
			return OK;
		} 
		else
		{	
			int result;
						
			if(type == TYPE_1LINE)
			{
				if( verbose )   cout << "** Conflict - two values assigned to " << name << endl;
				result = CONFLICT;
			}
			else
			{
				assert(type == TYPE_0LINE);
				if( verbose )   cout << "** Solution - two values sufficient for " << name << endl;
				result = QSAT;
			}

			if(reason == NULL)
			{
				assert(this == circuit->getRoot_node());
				circuit->resetLearnedSet(type, this);
			}
			else
				circuit->resetLearnedSet(type, reason );

			return result;
		}
	}

	assert( this->reason[new_value.type] == NULL );
	setValue( new_value.value, new_value.type, d_level, d_size, reason );

	if(new_value.justified)
		setJustified(true, type, reason);

	if( verbose ) {
		cout << "\tAssign " << name << " to " << valueToString( new_value.value ) 
			<< " on " << (type == TYPE_1LINE ? "1" : "0") << endl;
/*		
		cout << "          ";
		if(reason == NULL)
			cout << "Reason is NULL\n";
		else
			reason->print();
*/
}

	return OK;
}



/*
int DagNode::assign( int new_value, Reason * reason, bool& ignore, bool forced )
{
	ignore = false; //(*)

	if(!assigned)
	{
		if(universal)
		{
			if(gets ST or SF)
				assign opposite
			else // gets MT or MF
				conflict
		}
		else // existential
		{
			if(gets MT or MF)
				assign value
			else // gets ST or SF
				solution
		}
	}
	else // assigned
	{
		if(MT and VF or MF and VT)
		{
			conflict with my reason and coming reason
			return CONFLICT;
		}
		else if(ST and VT or SF and VF) // (*) Can you have both?
		{
			solution with my reason and coming reason
			return SOLUTION
		}
		else //just restated something already known
		{
			ignore = true;
			return OK;
		}

	}

	store undo record
	set the value
	etc.


	ddsfds f;
}*/

void DagNode::addToUPStack( DagNode * variable, Signal value, Reason * reason, bool forced )
{

	if(variable->getValue(value.type) != value.value)
	{
		// this is deleted when its popped
		circuit->up_stack->addNew( variable, value, reason, forced );
		// (*) More efficient memory management for all undo records!
		// (*) and for prop records!
	}
	else
	{
//		if(verbose)
//			cout << "Set " << variable->getName() << " Justified?\n";

		if(value.justified)
		{
			assert(!forced || (reason != NULL && reason->getSize() > 0));
			variable->setJustified(true, value.type, reason);
//			if(verbose)
//				cout << "Set " << variable->getName() << " Justified!\n";
		}
	}
}

void DagNode::addNotify(int value, short type, Notifiable *node)
{
	notify_list[value][type].push_back(node);
}

void DagNode::removeNotify(int value, short type, Notifiable* node)
{
	for(int i = 0; i < notify_list[value][type].size(); i++)
	{
		if (notify_list[value][type][i] == node)
		{
			notify_list[value][type][i] = notify_list[value][type][notify_list[value][type].size()-1];
			notify_list[value][type].pop_back();
			break;
		}
	}
}

void DagNode::initializeAllWatches()
{
    initializeTrueFalseWatches();
	initializeJustifiedWatches();
}

int DagNode::notifyWatches(int index, short type)
{
	assert(index == VALUE_TRUE || index == VALUE_FALSE || index == VALUE_DONT_CARE);
	int retval = OK;

    int i;
	for( i = 0; i < notify_list[index][type].size();)
	{
		bool remove;

		int size = notify_list[index][type].size(); // (*) remove for performance
		retval = notify_list[index][type][i]->notifyC( index, type, this, &remove );
		assert(notify_list[index][type].size() == size); // (*) 

		if(remove)
		{
			notify_list[index][type][i] = notify_list[index][type][notify_list[index][type].size() -1 ];
			notify_list[index][type].pop_back();
		}
		else
		{
			i++;
		}

		if( retval != OK ) 
		{
			return retval;
		}
	}

	return retval;
}

int DagNode::notifyWatches(short type)
{
	// Assumes that the list will not change while notifying
	assert(getValue(type) != VALUE_DONT_CARE);
	return notifyWatches(getValue(type), type);
}

bool DagNode::isJustified(short type)
{
    return dont_care || justified[type];
}

bool DagNode::isAllJustified()
{
	for(int t = 0; t < VAL_TYPES; t++)
	{
		if(!isJustified(t))
			return false;
	}
	return true;
}

// Reason: the child that justified this, "this" if all the children
// NULL if not justified
void DagNode::setJustified( bool new_justified, short type, Reason *reason )
{

	if(type == TYPE_BOTH)
	{
		if(!dont_care && new_justified)
		{
			circuit->undo_stack->addNew( UNDO_JUSTIFIED, this, type );
			if(verbose)
				cout << "\tDont Care " << name << endl;
			circuit->addJustified(this, type);
		}
		dont_care = new_justified;
		return;
	}

	if(dont_care)
	{
		assert(new_justified);
		return;
	}


    if( new_justified && !justified[type]) {
		circuit->undo_stack->addNew( UNDO_JUSTIFIED, this, type );
#ifndef TURN_OFF_DC
		if(verbose)
			cout << "\tJustified " << name
				<< " on " << (type == TYPE_1LINE ? "1" : "0") << endl;
		circuit->addJustified(this, type);
		assert(reason == NULL || reason->getSize() > 0);
#endif
	}

	justified[type] = new_justified;
}


void DagNode::forceJustified()
{
	dont_care = true;
	for(short t = 0; t < VAL_TYPES; t++)
		justified[t] = true;
}

int DagNode::notifyJustified( DagNode * justified, short type, bool *remove )
{
	int retval = OK;
	(*remove) = false;

	// we only care about this becoming DC if its not already assigned
	// (*) This is not necessarily true! This might prevent some children
	// of this node from becoming justified.
	// However, removing this would make it more expensive. (*)
	// TODO: (*) Investigate this tradeoff
	// Note: if remove this, need to come up with a way to recognize
	//   which children are responsible for parent becoming justified
	//   and don't propagate to them.
	if( isAssigned(type) || isJustified(type)) {
		return OK;
	}

	// search parents to find an unjustified one
	DagNode * new_watch = NULL;
	NodeList::iterator i;
	for( i = parents->begin(); i != parents->end(); i++ ) 
	{
		if( !(*i)->isJustified(type) ) {
			new_watch = *i;
			break;
		}
	}
	if( new_watch != NULL )
	{
		(*remove) = true;
		new_watch->addNotify( VALUE_DONT_CARE, type, this);
		justified_watch[type] = new_watch;

	} else {
		// all parents are justified
		//bool ignore;(*)
		//assign( VALUE_DONT_CARE, NULL, ignore, false ); (*) Never actually need value VALUE_DONT_CARE
		// TODO: (*) Remove all the references to VALUE_DONT_CARE, since it's never used - 
		// - replace with isJustified
		setJustified( true, TYPE_BOTH, NULL );
	}
	return retval;
}

void DagNode::initializeJustifiedWatches()
{
	// just pick the first one
	if( parents->size() > 0 ) {
		for(short t = 0; t < VAL_TYPES; t++)
		{
			justified_watch[t] = parents->at(0);
			justified_watch[t]->addNotify( VALUE_DONT_CARE, t, this);
		}
	}
}
