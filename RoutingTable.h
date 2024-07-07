#include <iostream>
#include <string>


class Machine;

class RoutingTable
{
public:
	Machine* current;
	RoutingTable* next;
	RoutingTable* previous;

	RoutingTable()
	{
		current = NULL;
		next = NULL;
		previous = NULL;
	}
};