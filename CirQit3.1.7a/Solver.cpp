#include "Solver.h"
#include "LeafNode.h"
#include "time.h"

#define VSIDS
#define LEARN_NO_GOODS

Solver::Solver(Circuit *c, bool verbose_n, unsigned time_out) : db(c)
{
	circuit = c;
	solver_time_out = time_out;
	d_level = 0;
	max_depth = 0;
	undo_ptr = 0;
	max_depth = 0;
	verbose = verbose_n;
	decisions_made = 0;
	curr_block = 0;
	c->up_stack = &up_stack;
	c->undo_stack = &undo_stack;
	c->justified_stack = &justified_stack;
	c->justified_types = &justified_types;
	c->verbose=verbose;
	last_type = TYPE_1LINE; // Shouldn't matter - no phases until first backtrack
}

// This forcing is for the root_node
void Solver::newDecisionLevel( DagNode * variable, int value, bool root)
{
	// (*) Check to see what is really necessary
	// this is one thing that keeps happening, no matter how long it takes to solve
	// so put the time out code here
	Common::checkTime();

	circuit->newDecisionLevel();

	// add a new level
	d_level++;
	if( d_level > max_depth ) {
		max_depth = d_level;
	}

	// store where this level's undo stack starts
	undo_ptrs.push( undo_ptr );
	undo_ptr = undo_stack.size();

	// record the decision variable and put it on up stack
	decision_vars.push_back( variable );
	decision_values.push_back( value );
	Signal val;
	val.value = value;
	val.justified = false;

	for(short t = 0; t < VAL_TYPES; t++)
	{
		val.type = t;
		if(root)
		{
			if(t == TYPE_1LINE)
				val.value = VALUE_TRUE;
			else if(t == TYPE_0LINE)
				val.value = VALUE_FALSE;
			else
				assert(false);
		}
		Reason *r = NULL;

		if(root)
		{
			circuit->beginNewReason(t, variable);
			r = circuit->last_reason;
		}

		up_stack.addNew( variable, val, r, root );
	}
}

void Solver::undoDecisionLevel()
{
	// (*) Check to see what is really necessary
	decision_vars.pop_back();
	decision_values.pop_back();

	if( verbose )   cout << "undoing decision level" << endl;

	// undo all records from this level
	UndoRecord * top;
	while( undo_stack.size() > (unsigned)undo_ptr )
	{
		undo_stack.undoAndPop();
	}

	// restore previous level's undo ptr
	undo_ptr = undo_ptrs.top();
	undo_ptrs.pop();

	d_level--;

	circuit->undoDecisionLevel();
}

int Solver::unitPropagate()
{
	if( verbose )   cout << "Unit propagating, level=" << d_level << endl;

	int retval = OK;
	while( !up_stack.empty() )
	{
		// propagate everything in the stack
		while( !up_stack.empty() )
		{
			retval = up_stack.propAndPop(this);

			if( retval != OK )
			{
				break;
			}
			if( retval != OK ) break;
		}

		if( retval != OK ) break;
		// check DB for conflict/qsat, or new forced literals
		retval = db.propagateDB();
		if( retval != OK ) break;
	}

	// clear the up_stack in case of CONFLICT/QSAT
	up_stack.clear();

	return retval;
}



int Solver::propagateJustified()
{
    int retval = OK;
    DagNode * top;
    while( !justified_stack.empty() ) {
        top = justified_stack.top();
        justified_stack.pop();
		short type = justified_types.top();
		justified_types.pop();
		if(type == TYPE_BOTH)
		{
			retval = top->notifyWatches( VALUE_DONT_CARE, TYPE_1LINE );
			retval = top->notifyWatches( VALUE_DONT_CARE, TYPE_0LINE );
		}
		else
			retval = top->notifyWatches( VALUE_DONT_CARE, type );
        assert( retval == OK );
    }
    return retval;
}


void Solver::resetStacks()
{
	while(!justified_stack.empty())
	{
		justified_stack.pop();
		justified_types.pop();
	}
	assert(justified_types.empty());
}

bool Solver::solve()
{
	// node and value keep the next chosen assignment
	DagNode * node;
	int value;

	solver_start_time = clock();
	int status = OK;

	// assign root to true
	newDecisionLevel( circuit->getRoot_node(), VALUE_TRUE, true );

	int d = 0;
	while(true)
	{
		if(false)
		{
			d++;
			if     (d % 10000 == 0)
				printf("\n"), fflush(stdout);
			else if(d % 1000 == 0)
				printf("!"), fflush(stdout);
			else if(d % 100 == 0)
				printf("."), fflush(stdout);
		}
		resetStacks();
		db.resetDB();

		status = unitPropagate();

#ifndef TURN_OFF_DC
		if( status == OK ) {
			status = propagateJustified();
		}
#endif


		if(status == CONFLICT)
		{
			// (*) use_solution_VSIDS = false;
			
			Literal *unit_implicant = circuit->lastLearnedSet->getImplicant();
			int level = circuit->lastLearnedSet->lastBTLevel(); 

			// This is enough for conflict
			if(unit_implicant == NULL || unit_implicant->variable->getDLevel(TYPE_1LINE) <= 1)
			{
				circuit->lastLearnedSet->getEmpty();
				return false;
			}

			if(level <= 0) // forcing a single literal
			{
				level = 1;
				// (*) Test
			}

			while( d_level > level )
			{
					undoDecisionLevel();
			}

			if( verbose )
			{
					cout << "Conflict: backtracking to level " << level << endl;
					cout << "Forcing variable to its opposite value: " << unit_implicant->variable->getName() 
						 << "=" << valueToString(opposite(unit_implicant->value)) << endl;
					string name = unit_implicant->variable->getName();
			}

			assert( !unit_implicant->variable->isUniversal(TYPE_1LINE) );

			Common::backtracks++;
			// addSet doesn't need unit implicant - it should be deducible while setting watches
			db.addSet( circuit->lastLearnedSet, TYPE_1LINE, true ); //(*) Clean up? don't need constant
			// (*) Collapse the two cases: conflicts and solutions

			last_type = TYPE_1LINE;
		}
		else if(status == QSAT || !chooseNextLiteral( node, value ))
		{
			assert(status == QSAT);

			/*
			if( d_level == 1 ) // no need to find a cube - we're done
			{
				circuit->lastLearnedSet->getEmpty();
				return true;
			}
			*/

			if(circuit->lastLearnedSet->empty())
			{
				assert(false);
				if( verbose ) cout << "finding a cube" << endl;
				//findCube();
			}

			Literal *unit_implicant = circuit->lastLearnedSet->getImplicant();

			int level = circuit->lastLearnedSet->lastBTLevel();

			// This is enough for QSAT
			if(unit_implicant == NULL || unit_implicant->variable->getDLevel(TYPE_0LINE) <= 1)
			{
				circuit->lastLearnedSet->getEmpty();
				return true;
			}

			if(level <= 0)
			{
				level = 1;
				// (*) Test
			}

			while( d_level > level )
			{
					undoDecisionLevel();
			}

			if( verbose ) cout << "QSAT: backtracking to level " << level << endl;

			if( verbose )
				cout << "Unit implicant: " << unit_implicant->variable->getName() << "=" << valueToString(opposite(unit_implicant->value)) << endl;

			Common::backtracks++;

			// addSet doesn't need unit implicant - it should be deducible while setting watches
			db.addSet( circuit->lastLearnedSet, TYPE_0LINE, true );
			last_type = TYPE_0LINE;
		}
		else
		{
			// No conflict or satisfaction, so continue branching
			newDecisionLevel(node, value);
		}
	}

	return true;
}

bool Solver::chooseNextLiteral( DagNode *& node, int &value)
{
	decisions_made++; //(*)?
	//if( ( literal_count % 500 ) == 0 ) cout << "dec vars: " << literal_count << endl;

    bool retval = false;
    if( (unsigned)curr_block >= circuit->numQuantifiers() ) return false;
    QuantifierBlock * block = circuit->getQuantifier( curr_block );

#if defined(VSIDS)
	// pick unassigned literal with highest combined VSIDS score
	int max_score = -1;
	int index = -1;

	for( int i = 0; i < block->num_vars; i++ )
	{
		DagNode * var = block->variables[i];
		// (*) Play with justified?
		if( var->isLeaf() )
		{
			LeafNode * leaf = (LeafNode*)var;
			if(!leaf->isDecision())
				continue;
			if( leaf->VSIDS_score[TYPE_1LINE] + leaf->VSIDS_score[TYPE_0LINE] > max_score )
			{
				max_score = leaf->VSIDS_score[TYPE_1LINE] + leaf->VSIDS_score[TYPE_0LINE];
				index = i;
				retval = true;
			}
		}
	}

	if(retval)
	{
		node = block->variables[index];

		if(Common::phase && (Common::phase_exist == node->isUniversal(last_type) ))
		{
			value = node->getPhase();
			assert(value == VALUE_TRUE || value == VALUE_FALSE);
		}
		else
		{
			value = VALUE_TRUE; // (*) Consider other values
		}
	}
//(*) Try SOLUTION_VSIDS

#else
	// (*) Other heuristics
	// this chooses them in order listed
    for( int i = 0; i < block->num_vars; i++ ) {
        if( !block->variables[i]->isAssigned() ) {
            node = block->variables[i];
            // this is ignored for now
            value = VALUE_TRUE;
            retval = true;
        }
    }
#endif

    if( !retval ) {
        // current block is exhausted - change to next
        // undo records are deleted by the undo mechanism
        undo_stack.addNew( curr_block, this );
        curr_block++;
        retval = chooseNextLiteral( node, value );
    }
    return retval;
}

void Solver::printStats()
{
	cerr << "Number of decision nodes: " << decisions_made << endl;
	cerr << "Backtracks: " << Common::backtracks << endl;
    cerr << "Maximum search depth: " << max_depth << endl;
	//(*)
}
