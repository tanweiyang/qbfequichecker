#ifndef _QdToQpro_H_
#define _QdToQpro_H_

#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

/* If SubFmla t, t[0] is ID. ID > 0 is disjunction, < 0 is conjunction,
 * with ID == -(n+1) being the top level quantifier-free formula.
 * |ID|-n is index in SubFmlas object.  t[1] is number of elements.
 * Elements e[j], j=2...k=1 are literals if |e[j] <= n else SubFmla IDs.
 */
typedef vector<int> SubFmla;
typedef vector<SubFmla> SubFmlas;

/* If Qblock b, b[0] is qdepth, qdepth > 0 is existential, < 0 is universal,
 * and |qdepth| is quantifier depth, beginning with 1.
 * b[1] is number of vars in Qblock, rest is vars.
 */
typedef vector<int> Qblock;
typedef vector<Qblock> Qblocks;

/* If Where  w, w[0] is lit, w[1] is number of occs, rest is SubFmla IDs
 * where lit appears at "top" level.  Separate Wheres for pos. and neg.
 */
typedef vector<int> Where;
typedef vector<Where> Wheres;

/* Qdepths is indexed by var. +qdepth means exists, -qdepth means univ. */
typedef vector<int> Qdepths;

class QdToQpro
{
private:
	int linenum;


	/* return Id in subfs of the top-level subformula, after possible
	 * restructuring into a logically equivalent form.
	 * Precondition: The initial format is unsimplified CNF.  E.g.,
	 * a unit clause or empty clause is still a disjunction.
	 * Postcondition: no variables are eliminated.
	 */
	int reworkAndOr(SubFmlas &subfs, int subfId, int vars,
	Wheres &posOccs, Wheres &negOccs, Qblocks &scopes, Qdepths &scopeof);

	/* print and_or tree to sqpro */
	bool printAndOr(SubFmlas &subfs, int subfId, int vars);

	/* Set up sqpro to contain qpro equivalent of qdimacs infile
	 * with  some and-or tree structure based on collecting vars.
	 * Return true if successful.
	 */
	bool parse_qdimacs(ifstream &infile);

	/* Set up sqpro to contain qpro equivalent of qdimacs infile.
	 * Return true if successful.
	 */
	bool simple_parse_qdimacs(ifstream &infile);

	// Read in the list of signed integers, appending to vector
	// ASSUMING ws excludes '\n'.  Consume the '\n'.
	void readIntList(vector<int> &vec, ifstream &infile);


public:
	/* After init returns true, sqpro can be used as an istream that
	 * delivers a qpro-format qbf formula.
	 */
	stringstream sqpro;

	QdToQpro();

	// How do I destroy sqpro to be sure the space is freed? TODO
	~QdToQpro();

	// Returns the number of the current line
	int getLineNum();

	/* Set up sqpro to contain qpro equivalent of qdimacs infile.
	 * Return true if successful.
	 */
	bool init(char *filename);

};


#endif

/* Intended use:  first construct a QdToQpro object, say qq.  Then call
 * qq.init(qdimacs_file).  Then pass qq.sqpro to parse_qpro() as though
 * it were an fstream.  Let parse construct the circuit, etc., and finish.
 * Finally, do "delete qq" and hope that it deletes sqpro.
 * Proceed with solving.
 */

