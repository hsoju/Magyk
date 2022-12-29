#include "Force.h"
#include "Hooks.h"

void Hooks::Install() {
	Magyk::Force::InstallHooks();
	logger::info("Installed all hooks.");
}
