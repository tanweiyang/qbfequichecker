#include "Literal.h"


map<DagNode*,Literal**> vars_to_literals;

// (*) Try just creating; they are light in this version - might be faster
// look up the literal, or create it if it doesn't exist
Literal * getLiteral( DagNode * variable, int value )
{
	assert(value != VALUE_UNASSIGNED);
	Literal ** lits;

	map<DagNode*,Literal**>::iterator i = vars_to_literals.find(variable);
	if( i == vars_to_literals.end() )
	{
		lits = new Literal* [2];
		lits[0] = lits[1] = NULL;
		vars_to_literals.insert( make_pair( variable, lits ) );
	}
	else
		lits = i->second;

	if(lits[value] == NULL)
	{
		lits[value] = new Literal(variable, value);
	}

	return lits[value];
}
