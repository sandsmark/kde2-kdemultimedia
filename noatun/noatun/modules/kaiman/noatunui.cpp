#include "userinterface.h"

extern "C"
{
        Plugin *create_plugin()
        {
                return new Kaiman();
        }
}
