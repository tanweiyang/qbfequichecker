
#ifndef LITERAL_H
#define LITERAL_H

#include "Common.h"
#include "DagNode.h"
#include <map>
using namespace std;

class Literal
{
public:
	DagNode *variable;
	int value;
	Literal(DagNode* var, int val) : variable(var), value(val)
	{
		assert(value == VALUE_TRUE || value == VALUE_FALSE);
	}
	inline bool isTrue(short type){ return variable->getValue(type) == value; };

	void proofPrint()
	{
		if(value == VALUE_FALSE)
			Common::proof_file << "-";
		Common::proof_file << variable->getName();
	}
};

// (*) Try just creating; they are light in this version - might be faster
// look up the literal, or create it if it doesn't exist
Literal * getLiteral( DagNode * variable, int value );

#endif
