#include "Caster.h"
#include "Force.h"
#include "Hooks.h"
#include "Jumper.h"

void InitializeSettings() {
	auto force_handler = Magyk::Force::GetSingleton();
	force_handler->SetConfig();
	force_handler->GetFallDamage();
	auto caster_handler = Magyk::Caster::GetSingleton();
	caster_handler->GetBlacklist();
	caster_handler->GetWhitelist();
}