#include "systray.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new NoatunSystray();
	}
}
