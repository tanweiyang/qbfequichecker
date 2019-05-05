#include "Tester.h"

/*

void Tester::addToUP(string svariable, string svalue)
{
	DagNode *var = circuit->lookupVariable(svariable);

	int value = stringToValue(svalue);

	newDecisionLevel( var, value );

	cout << "Set " << svariable << " = " << valueToString(var->getValue()) << "\n";
}

void Tester::printValue(string svariable)
{
	DagNode *var = circuit->lookupVariable(svariable);

	cout << "Now " << svariable << " = " << valueToString(var->getValue()) << "\n";
}

void Tester::propagate()
{
	cout << "~~~~~~~~~~~~~~~~~~Propagating~~~~~~~~~~~~~~~~\n";
	circuit->resetLearnedSet();
	unitPropagate();
}

void Tester::addLearnedSet(string type, vector<string> varsAndVals)
{
	circuit->resetLearnedSet();
	for(int i = 0; i < varsAndVals.size(); i+=2)
	{
		assert(i + 1 < varsAndVals.size());
		circuit->addToLearnedSet(new Literal(circuit->lookupVariable(varsAndVals[i]),
			stringToValue(varsAndVals[i+1])));
	}

	int t;
	if(type == "CLAUSE")
		t = SET_CLAUSE;
	else
	{
		assert(type == "CUBE");
		t = SET_CUBE;
	}

	db.addSet(circuit->lastLearnedSet, t,false);
}

Tester::Tester(Circuit *c) : Solver(c, true)
{
	bool ret = c->initializeAndVerify( c->getRoot_node() );
	assert(ret);
}
*/
