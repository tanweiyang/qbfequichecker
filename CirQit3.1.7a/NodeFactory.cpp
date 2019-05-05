
#include <string>

#include "NodeFactory.h"

#include "AndOrGate.h"
#include "NotGate.h"
//#include "DummyNode.h"

#include "LeafNode.h"
#include "Allocator.h"

Allocator * node_allocator = NULL;
Allocator * dummy_node_allocator = NULL;


void initNodeFactory()
{
    node_allocator = new Allocator();
    dummy_node_allocator = new Allocator();
}


void deleteDummyNodes()
{
    delete(dummy_node_allocator);
    dummy_node_allocator = NULL;
}


void deleteAllNodes()
{
    delete(node_allocator);
    node_allocator = NULL;
}


DagNode * createGate( GATE_TYPE type, string &name, Circuit *c )
{
	assert(node_allocator != NULL);

	switch( type )
	{

	case NOT_GATE:
		return new(node_allocator) NotGate(name, c);
	case AND_GATE:
	case OR_GATE:
		return new(node_allocator) AndOrGate(type, name, c);
	default:
		assert( false ); // Unexpected gate type
		return NULL;
	}
}

DagNode * createLeafNode( string &name, Circuit *c )
{
	return new(node_allocator) LeafNode( name, c );
}
