#include "Reason.h"
#include "Notifiable.h"
#include "Literal.h"


Reason::Reason(DagNode* noutput, int linetype)
{
	int value = (linetype == TYPE_1LINE? VALUE_FALSE : VALUE_TRUE);
	root_lit = getLiteral(noutput, value);
	output = true;
}

int Reason::getSize()
{
	if(output)
	{
		return 1;
	}
	return extern_reason.source->getClauseSize(extern_reason.child);
}

Literal *Reason::litAt(int index)
{
	if(output)
		return root_lit;
	return extern_reason.source->getClauseLiteral(extern_reason.child, index);
}

void Reason::printProofClause()
{
	if(output)
		Common::proof_file << "o";
	else
		extern_reason.source->printProofClause(extern_reason.child);
}

void Reason::print()
{
	cout << "Reason: ";
	for(int i = 0; i < getSize(); i++)
	{
		if(i > 0)
			cout << ", ";
		Literal* lit = litAt(i);
		cout <<  lit->variable->getName() << '=' << valueToString(lit->value);
	}
	cout << "\n";
}

