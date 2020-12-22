#include <kcmodule.h>

#include "playlist.h"


extern "C"
{
	Plugin *create_plugin()
	{
		return new SplitPlaylist();
	}
}

