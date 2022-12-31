#include "Hooks.h"
#include "Force.h"
#include "Caster.h"
#include "Jumper.h"

void Hooks::Install() {
	Magyk::Force::InstallHooks();
	Magyk::Caster::AddSpellCastSink();
	Magyk::Jumper::AddJumpSink();
	logger::info("Installed all hooks.");
}
