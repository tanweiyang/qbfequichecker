#include "Circuit.h"
#include "LeafNode.h"

extern bool silent;

DagNode *Circuit::addInput(string &var_name)
{
	DagNode *var = lookupVariable( var_name );

	if( var == NULL ) {
		var = createLeafNode( var_name, this );
		storeVariable( var_name, var );
		inputs.push_back((LeafNode *)var);
	}

	return var;
}

void Circuit::addInputs(vector<string> &list)
{
	for(unsigned i = 0; i < list.size(); i++)
	{
		addInput(list[i]);
	}
}

void Circuit::addOutputs(vector<string> &list)
{
	for(unsigned i = 0; i < list.size(); i++)
		addOutput(list[i]);
}

void Circuit::addOutput(string name)
{
	outputs.push_back(name);
}

void Circuit::addGate(string name, int type, vector<string> &children)
{
	DagNode * gate = createGate( type, name, this );
	storeVariable( name, gate);
	gate->setDummyChildren(children);
}

void Circuit::addQuantifier(vector<string> &list, int type)
{
	QuantifierBlock * block = NULL;

	if( quantifiers.size() > 0 )
	{
		block = quantifiers.back();
	}

	if( block != NULL && block->type == type )
	{
		// add these variables to the previous block

		int new_size = block->num_vars + list.size();
		DagNode ** new_list = new DagNode *[new_size];

		int i;
		// copy block's vars into the new list
		for( i = 0; i < block->num_vars; i++ )
		{
			new_list[i] = block->variables[i];
		}

		// copy new vars into the list
		addList(new_list, list, type, quantifiers.size(), i);

		// replace old list
		delete[] block->variables;
		block->variables = new_list;
		block->num_vars = new_size;
	}
	else
	{
		// create a new block
		DagNode ** n_list = new DagNode *[list.size()];

		addList(n_list, list, type, quantifiers.size() + 1, 0);

		block = new QuantifierBlock();
		block->type = type;
		block->variables = n_list;
		block->num_vars = list.size();

		quantifiers.push_back( block );
	}
}

void Circuit::printQBF(DagNode* curr)
{
	if(curr == NULL)
	{
		int brackets = 0;
		for(int i = 0; i < quantifiers.size(); i++)
		{
			brackets++;
			cout << "(";

			if(quantifiers[i]->type == EXISTENTIAL)
				cout << "exists ";
			else
			{
				assert(quantifiers[i]->type == UNIVERSAL);
				cout << "forall ";
			}

			cout << "[";
			for(int j = 0; j < quantifiers[i]->num_vars; j++)
			{
				if((quantifiers[i]->variables[j])->isLeaf())
				{
					cout << " ";
					cout << (quantifiers[i]->variables[j])->getName();
				}
			}

			cout << " ]";
		}
		cout << " ";
		printQBF(root_node);
		while(brackets--)
			cout << ")";
	}
	else
	{
		if(curr->isLeaf())
		{
			cout << curr->getName();
		}
		else
		{
			cout << "(";
			
			int type = curr->getType();

			if(type == NOT_GATE)
			{
				cout << "!";
			}

			int num_children = curr->getNumChildren();
			DagNode * * children = curr->getChildren();
			int closing = 0;

			for(int i = 0; i < num_children; i++ )
			{
				if(i > 0)
				{
					if(type == AND_GATE)
						cout << " & ";
					else if (type == OR_GATE)
						cout << " | ";
					else
						assert(false); // invalid gate type 
						               //  or multiple children for NOT gate
					if(i < num_children - 1)
					{
						// Another expression follows
						closing++;
						cout << "(";
					}
				}

				printQBF(children[i]);
			}

			while(closing--)
				cout<<")";
			cout << ") ";
		}
	}
}


void Circuit::finalize()
{
	// make the output list into the root (AND them together if more than one)
	assert(outputs.size() > 0);
	if(outputs.size() == 1)
		root_node = lookupVariable(outputs[0]);
	else
	{
		string name = "Root";
		while(lookupVariable(name) != NULL)
		{
			name = name + "0";
		}
		addGate(name, AND_GATE, outputs);
		root_node = lookupVariable(name);
	}

	outputs.clear();


	// initialize the indexed list of inputs, and assign scores to inputs (for cubes)
	VariableMap::iterator i;
	for( i = variable_map.begin(); i!= variable_map.end(); i++ )
	{
		if( i->second->isLeaf() ) {
			LeafNode * input = (LeafNode*)i->second;
			if( input->isUniversal(TYPE_1LINE) ) {
				input->cube_score = input->getBlockNum() / 2;
			} else {
				input->cube_score = (input->getBlockNum() + quantifiers.size() + 1) / 2;
			}
			//input->index = input_count++;
			//inputs.push_back( input );
	        
			// initialize VSIDS score (2 clauses for every parent)
			input->VSIDS_score[TYPE_1LINE] = input->getNumParents() * 2; //(*) Figure out heuristics
			input->VSIDS_score[TYPE_0LINE] = 0 ;//(*) What to set to?

		}
		//assert( (unsigned)input_count == inputs.size() );
	}

	// --------------------------------------------------
	// optimize and preprocess the dag
	// --------------------------------------------------

	// build the Dag by filling in dummy pointers
	root_node = buildDag(root_node);

	// initialize the indexed list of inputs, and assign scores to inputs (for cubes)
	// (*) Do at the end of preprocessing

	// Set the block numbers
	// (*) set root block number to 0?
	root_node->setBlockNum(true);

	// (*) include all initializations
	reason_ptr = 0;
	d_level_alloc = NULL;
	lastLearnedSet = NULL;
}

DagNode *Circuit::buildDag(DagNode *curr)
{
	DagNode *result = curr;
	if(curr->childNames == NULL)
		return result; // Already finished with this node, or it is a leaf

	vector<string> &names = *(curr->childNames);

	NodeList * new_child_list = new NodeList();
	int num_children = curr->getNumChildren();

	for(int i = 0; i < names.size(); i++)
	{
		DagNode *child = lookupVariable(names[i]);
		if(child == NULL)
			cerr << "Can't find the child " << names[i] << "\n";
		assert(child != NULL);
		child = buildDag(child);

		new_child_list->push_back( child );
	}
	curr->setChildren(new_child_list);

	delete(curr->childNames);
	curr->childNames = NULL;

	return result;
}


int Circuit::getHashValue( DagNode * node )
{
	int hash_val = node->getType();
	int n = node->getNumChildren();
	DagNode ** children = node->getChildren();
	for( int i = 0; i < n; i++ )
	{
		hash_val += (long)(children[i]);
	}
	return hash_val % 1021;
}

// Return the hast value of the negation of the node
int Circuit::getHashValueNot( DagNode * node )
{
	int hash_val = NOT_GATE;
	hash_val += (long)(node);
	return hash_val % 1021;
}

// Return the hast value of the negation of the node
int Circuit::getHashValueNeg( DagNode * node )
{
	return 0;
}


// For all children 
void Circuit::mergeCommonSubformulas(DagNode *curr)
{
	if( visited.find( curr ) != visited.end() ) return;
    else visited.insert( make_pair( curr, 0 ) );
    
	if( curr->isLeaf() ) return;

	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();
	for(int i = 0; i < num_children; i++ )
	{
		mergeCommonSubformulas(children[i]);
	}

	map<int,NodeList*>::iterator i;
	int hash_val = getHashValue( curr );
	i = hash_map.find( hash_val );

	if( i == hash_map.end() )
	{
		// it doesn't match, so add it
		NodeList * hash_entry = new NodeList();
		hash_entry->push_back( curr );
		hash_map.insert( make_pair( hash_val, hash_entry ) );
	}
	else
	{
		NodeList * hash_entry = i->second;
		// go through the list and check for any exact matches
		NodeList::iterator j;
		bool merged = false;
		for( j = hash_entry->begin(); j != hash_entry->end(); j++ )
		{
			if( compareNodes( curr, *j ) )
			{
				// merge p into j
				mergeNodes( *j, curr );
				merged = true;
				break;
			}
		}
		if( !merged )
		{
			hash_entry->push_back(curr);
		}
	}
}

bool compareNodes( DagNode * a, DagNode * b )
{
	if( a->getNumChildren() != b->getNumChildren() ) 
		return false;

	if( a->getType() != b->getType() )
		return false;

	// put a's children in a map
	DagNode ** a_children = a->getChildren();
	map<DagNode*,char> child_map;
	for( int i = 0; i < a->getNumChildren(); i++ ) {
		child_map.insert( make_pair( a_children[i], 0 ) );
	}

	// check if all of b's children exist in the map
	DagNode ** b_children = b->getChildren();
	for( int i = 0; i < b->getNumChildren(); i++ ) {
		map<DagNode*,char>::iterator j = child_map.find( b_children[i] );
		if( j == child_map.end() ) return false;
		else child_map.erase( j );
	}

	return true;
}

void Circuit::mergeNodes( DagNode * a, DagNode * b )
{  
	if( a == b )
	{
		// they are the same node - they'll be removed in removeDupes stage
		return;
	}

	// merge b into a 
	NodeList::iterator i;
	for( i = b->getParents()->begin(); i != b->getParents()->end(); i++ )
	{
		// replace b with a in child list of parent
		DagNode ** children = (*i)->getChildren();
		int num_children = (*i)->getNumChildren();
		for( int j = 0; j < num_children; j++ ) {
			if( children[j] == b )
			{
				children[j] = a;
				//break;
			}
        }

		// add parent to a
		a->addParent(*i);
	}

	if(root_node == b)
		root_node = a;

	// remove b from its children
	DagNode ** children = b->getChildren();
	int num_children = b->getNumChildren();
	for( int j = 0; j < num_children; j++ ) {
		// remove b from parent list
		children[j]->removeParent(b);
	}
	//delete b;

}

void Circuit::removeDupes(DagNode *curr)
{
	if( visited.find( curr ) != visited.end() ) return;
	else visited.insert( make_pair( curr, 0 ) );

	// make sure no dupes in child list
	map<DagNode*,char> child_map;
	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();

	for( int i = 0; i < num_children; i++ ) {
		if( child_map.find( children[i] ) == child_map.end() ) {
			child_map.insert(make_pair( children[i], 0 ) );
		}
	}

	NodeList * new_child_list = new NodeList();


	map<DagNode*,char>::iterator j;
	int i = 0;
	for( j = child_map.begin(); j != child_map.end(); j++ )
	{
		new_child_list->push_back(j->first);
	}
	curr->setChildren(new_child_list);

	// make sure no dupes in parent list
	NodeList::iterator k;
	NodeList * parents = curr->getParents();
	map<DagNode*,char> parent_map;
	for( k = parents->begin(); k != parents->end(); k++ ) {
		if( parent_map.find( *k ) == parent_map.end() ) {
			parent_map.insert(make_pair( *k, 0 ) );
		}
	}
	parents->clear();
	for( j = parent_map.begin(); j != parent_map.end(); j++ ) {
		parents->push_back( j->first );
	}

	num_children = curr->getNumChildren();
	children = curr->getChildren();
	if( num_children == 1 && curr->getType() != NOT_GATE ) {
		DagNode* firstChild = (curr->getChildren())[0];
		// the gate should be replaced with its child
		if( curr == root_node ) {
			root_node = firstChild;
		} else {
			for( k = parents->begin(); k != parents->end(); k++ ) {
				bool replaced = false;
				for( i = 0; i < (*k)->getNumChildren(); i++ ) {
					DagNode * * children = (*k)->getChildren();
					if( children[i] == curr ) {
						children[i] = firstChild;
						replaced = true;
						break;
					}
				}
				firstChild->addParent(*k);
				assert(replaced);
			}
		}
	}
	
	for( int i = 0; i < num_children; i++ )
	{
		removeDupes( children[i] );
	}
}

// This should be called instead of clearing the visited list
bool Circuit::countParents(DagNode *curr, bool remove)
{
	map<DagNode*, char>::iterator k_curr = visited.find(curr);
	if( k_curr == visited.end() ) return true;
	
	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();

	for( int i = 0; i < num_children; i++ ) {
		if( !countParents( children[i], remove ) ) 
			return false;
	}

	NodeList *parents = curr->getParents();
	// We visit children first, so my parents have not yet been visited

	for(int i = 0; i < parents->size(); i++)
	{
		DagNode *p = parents->at(i);
		if(visited.find(p) == visited.end())
		{
			assert(remove);
			curr->removeParent(p);
			return countParents(curr, remove);
		}
	}

	visited.erase(k_curr);
	return true;
}

bool Circuit::initializeAndVerify( DagNode * curr )
{
	if( visited.find( curr ) != visited.end() ) return true;
	else visited.insert( make_pair( curr, 0 ) );

	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();

	// check for valid # of children
	//if( !curr->verifyStructure() ) return false; (*)

	curr->initializeAllWatches();

	for( int i = 0; i < num_children; i++ ) {
		assert(children[i]->containsParent(curr));
		if( !initializeAndVerify( children[i] ) ) 
			return false;
	}

	return true;
}

bool Circuit::preprocess()
{
	hash_map.clear();
	visited.clear();

	if(Common::preprocess)
	{
		// merge common subformulas, same-typed gates, remove double negation,
		// and remove any dupes created in parent/child lists
		do
		{
			visited.clear();
			mergeCommonSubformulas(root_node);
			// Delete temporary lists
			for(map<int,NodeList*>::iterator i = hash_map.begin(); i != hash_map.end(); i++)
			{
				delete i->second;
			}
			hash_map.clear();
			visited.clear();
			removeDupes( root_node );
			visited.clear();
		}
		while(simplify(root_node));
		
		countParents(root_node, true); // This also clears visited
		visited.clear();
		removeDupes( root_node );

		if(Common::larify)
		{
			visited.clear();
			larify( root_node );
		}
	}
	else
	{
		removeDupes( root_node );
		//countParents(root_node, false);
		//visited.clear();
	}

	countParents(root_node, false); // This also clears visited
	visited.clear();

	// initialize watch structures and verify dag structure is sound

	visited.clear();

	if( !initializeAndVerify( root_node ) ) return false; //(*)
	
	if(!TO_QBF && !silent)
	{
		cout << "num vars = " << variable_map.size() << endl;
		cout << "reduced to " << visited.size() << endl;
	}

	// update the variable map to include only those variables 
	// that are remaining
	for(VariableMap::iterator iter=variable_map.begin(); iter != variable_map.end();)
	{
		DagNode *curr = &(*iter->second);
		if(visited.find(curr) != visited.end() )
		{
			iter++;
		}
		else
		{
			if(curr->isLeaf())
			{
				curr->forceJustified();
			}
			variable_map.erase(iter++);
		}
	}

	return true;
}

// Merge same gates, remove double negation
bool Circuit::simplify(DagNode *curr)
{
	bool result = false;

	if( visited.find( curr ) != visited.end() ) return result;
	else visited.insert( make_pair( curr, 0 ) );

	bool changedList = false;

	NodeList * new_child_list = new NodeList();
	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();

	// For every child
	for(int i = 0; i < num_children; i++)
	{
		// Simplify child
		result = simplify(children[i]) || result;
		
		if(children[i]->getType() == curr->getType() && curr->getType() != NOT_GATE)
		{
			// Same type: can be merged
			result = true;
			changedList = true;

			DagNode ** child_inputs = children[i]->getChildren();
			int num_inputs = children[i]->getNumChildren();
			for( int j = 0; j < num_inputs; j++ ) {
				new_child_list->push_back( child_inputs[j] );
				child_inputs[j]->removeParent(children[i]);
			}
		}
		else if(children[i]->getType() == NOT_GATE && curr->getType() == NOT_GATE)
		{
			// Double negation
			result = true;
			// Here curr = NOT(child)
			// child = NOT(something)
			// So, curr should be replaced by 'something'
			// Which is the child's child
			// To remove NOT(NOT(something))
			assert(num_children == 1);
			assert(children[i]->getNumChildren() == 1);
			DagNode *newMe = (children[i]->getChildren())[0];
			mergeNodes(newMe, curr);
			// Child list stays the same
		}
		else
		{
			new_child_list->push_back( children[i] );
		}
	}
	if(changedList)
		curr->setChildren(new_child_list);
	else
		delete new_child_list;

	return result;
}


void Circuit::larify(DagNode *curr)
{
	if( visited.find( curr ) != visited.end() ) return;
	else visited.insert( make_pair( curr, 0 ) );
	int i;

	vector<NodeList *> new_child_list(quantifiers.size()+1, NULL);
			// Quantifiers are 1-indexed with 0 reserved for root

	int num_children = curr->getNumChildren();
	DagNode * * children = curr->getChildren();

	int blocks = 0;
	// For every child
	for(i = 0; i < num_children; i++)
	{
		larify(children[i]);

		// for intermediate nodes, only care about its descendant leaves
		//   By convention, leaves are at even level, nodes at odd just above leaves
		int block = children[i]->getBlockNum() / 2;
		if(new_child_list[block] == NULL)
		{
			new_child_list[block] = new NodeList();
			blocks++;
		}
		new_child_list[block]->push_back(children[i]);
	}

	DagNode *prevGate = NULL;
	// Want to have quantifier-earlier gates
	//     unrelated to the deeper levels
	for(i = 0; i < new_child_list.size(); i++)
	{
		if(new_child_list[i] != NULL)
		{
			blocks--;

			if(new_child_list[i]->size() == 1 && prevGate == NULL)
				prevGate = new_child_list[i]->at(0);
			else
			{
				DagNode *newGate;
				if(blocks == 0)
					newGate = curr; // Last gate should be the original
				else
				{	
					string name = string(curr->getName()) + "[lvl " + toSt(i) + "]";
					   // No conflicts because assume original names don't have spaces
					
					// Create a gate for level i and shallower: the children and prevGate
					newGate = createGate(curr->getType(),name,this);
					storeVariable(name, newGate);
				}

				if(prevGate != NULL)
					new_child_list[i]->push_back(prevGate);

				for(int j =0; j < new_child_list[i]->size(); j++)
					new_child_list[i]->at(j)->removeParent(curr);

				newGate->setChildren(new_child_list[i]);
				new_child_list[i] = NULL; // setChildren deletes the list

				newGate->setBlockNum(newGate == root_node, true);
				assert(newGate == root_node ||
					(newGate->getBlockNum() == 2*i+1));

				prevGate = newGate;
			}
		}
	}

}





void Circuit::printIscasCircuit( DagNode * curr, ofstream &fout )
{
	if( curr->isLeaf() ) return;

	if( visited.find( curr ) != visited.end() ) return;
	else visited.insert( make_pair( curr, 0 ) );

	fout << curr->getName() << " = ";
	switch( curr->getType() ) 
	{
		case NOT_GATE: fout << "NOT( ";    break;
		case AND_GATE: fout << "AND( ";    break;
		case OR_GATE:  fout << "OR( ";     break;
		default:       assert( false );
	}
	DagNode ** children = curr->getChildren();
	int num_children = curr->getNumChildren();
	for( int i = 0; i < num_children; i++ ) {
		if( i != 0 ) fout << ", ";
		fout << children[i]->getName();
	}
	fout << " )" << endl;


	for( int i = 0; i < num_children; i++ ) {
		printIscasCircuit( children[i], fout );
	}
}


void Circuit::printIscas(ofstream &fout)
{
	vector<QuantifierBlock*>::iterator i;
	for( i = quantifiers.begin(); i != quantifiers.end(); i++ ) {
		if( (*i)->type == EXISTENTIAL ) fout << "EXISTS( ";
		else fout << "FORALL( ";
		for( int j = 0; j < (*i)->num_vars; j++ ) {
			if( j != 0 ) fout << ", ";
			fout << (*i)->variables[j]->getName();
		}
		fout << " )" << endl;
	}


	vector<DagNode*>::iterator k;
	fout << "INPUT( ";

	VariableMap::iterator iter;
	iter = variable_map.begin();
	bool first = true;
	while(iter != variable_map.end())
	{
		if(iter->second->isLeaf())
		{
			if(!first)
				fout << ", ";
			else
				first = false;
			
			fout << iter->first;
		}
		iter++;
	}
	fout << " )" << endl;;

	// print output
	fout << "OUTPUT( " << root_node->getName() << " )" << endl;

	// print gates
	visited.clear();
	printIscasCircuit( root_node, fout );
}

void Circuit::newDecisionLevel()
{
	allocator_stack.push( d_level_alloc );
	d_level_alloc = new Allocator();
}

void Circuit::undoDecisionLevel()
{
	// delete everything at this level, and go back to previous level
	delete(d_level_alloc);
	d_level_alloc = allocator_stack.top();
	allocator_stack.pop();
}

void Circuit::addJustified(DagNode *d, short type)
{
	justified_stack->push(d);
	justified_types->push(type);
}

void Circuit::decayVSIDS(int type)
{
	for(int i = 0; i < inputs.size(); i++)
	{
		inputs[i]->decayVSIDS(type);
	}
}


void Circuit::beginNewReason(Notifiable *source, DagNode* child)
{
	last_reason = new(d_level_alloc) Reason(source, child);
}

void Circuit::beginNewReason(int linetype, DagNode* root)
{
	last_reason = new(d_level_alloc) Reason(root, linetype);
}



void Circuit::resetLearnedSet(int linetype, Notifiable *source, DagNode *child )
{
	Reason r(source, child);
	resetLearnedSet(linetype, &r);
}

void Circuit::resetLearnedSet(int linetype, DagNode *root )
{
	Reason r(root, linetype);
	resetLearnedSet(linetype, &r);
}

void Circuit::resetLearnedSet(int linetype, Reason* conflict_clause)
{
	if(lastLearnedSet == NULL)
		lastLearnedSet = new ComReason();

	lastLearnedSet->clear(linetype, conflict_clause);
}

