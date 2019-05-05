#ifndef REASON_H
#define REASON_H

class Literal;
class Notifiable; 
class DagNode;

class Reason
{
private:
	union
	{
		struct
		{
			Notifiable* source;
			DagNode* child;
		} extern_reason;
		Literal* root_lit;
	};
	int position;
	bool output;

public:
	// Special case: is source is the output,
	//   then the clause represented is the special clause
	//       "output" (is child is output) or "!output" (if child is NULL)
	Reason(Notifiable* nsource, DagNode* nchild)
	{
		extern_reason.source = nsource;
		extern_reason.child = nchild;
		position = 0;
		output = false;
	}

	// Create a reason that is "output" for linetype TYPE_1LINE,
	//   and "!output" for linetype TYPE_0LINE
	Reason(DagNode* output, int linetype);

	// Returns the number of literals in this reason
	int getSize();

	// Returns the literal at the given index
	Literal *litAt(int index);

	// Prints an identifier of this reason to the proof file output
	void printProofClause();

	void print();
};



#endif
