#include <kglobal.h>
#include <klocale.h>

#include "winskin.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new winskin();
	}
}
