#include "Hooks.h"
#include "Force.h"
#include "Caster.h"
#include "Jumper.h"

void Hooks::InstallCaster() {
	Magyk::Caster::GetSingleton()->AddSpellCastSink();
	logger::info("Installed caster hook.");
}

void Hooks::InstallInputs() {
	Magyk::Jumper::GetSingleton()->AddJumpSink();
	Magyk::Force::Register();
	logger::info("Installed input hook.");
}
