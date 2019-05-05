#ifndef _nNODE_FACTORY_H_
#define _nNODE_FACTORY_H_

#include "Common.h"
#include "DagNode.h"
#include "Circuit.h"
#include <string>

#define GATE_TYPE       int
#define AND_GATE        2
#define OR_GATE         3
#define NOT_GATE        4


DagNode * createGate( GATE_TYPE type, string &name, Circuit *c );
DagNode * createLeafNode( string &name, Circuit *c );

// must be called first (creates allocators)
void initNodeFactory();
void deleteDummyNodes();
void deleteAllNodes();

#endif
