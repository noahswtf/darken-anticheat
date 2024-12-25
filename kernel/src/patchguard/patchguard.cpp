#include "patchguard.h"

extern "C" void _trigger_patchguard_bugcheck();

void patchguard::trigger_bugcheck()
{
	_trigger_patchguard_bugcheck();
}
