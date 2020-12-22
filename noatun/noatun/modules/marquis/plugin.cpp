#include "marquis.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new Marquis();
	}
}
