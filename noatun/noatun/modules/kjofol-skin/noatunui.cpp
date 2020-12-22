#include "kjloader.h"
#include <player.h>

extern "C"
{
	Plugin *create_plugin()
	{
		return new KJLoader();
	}
}
