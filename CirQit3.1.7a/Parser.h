#ifndef _pPARSER_H_
#define _pPARSER_H_

#include <iostream>
#include "Common.h"
#include "Circuit.h"

/*
Assumes the contents of the file "filename" are in ISCAS format
Parses the information and stores it in circuit
Returns true if parsed successfully,
False on error (after outputting the error message to cerr)
*/
bool parse(char * filename, Circuit &circuit);

bool testFile(char * filename);

#endif
