#include "Caster.h"

void Magyk::Caster::AddSpellCastSink() {
	auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
	if (sourceHolder) {
		sourceHolder->AddEventSink(Caster::GetSingleton());
	}
}

bool Magyk::Caster::IsValidSpell(RE::SpellItem* a_spell) {
	if (a_spell->GetDelivery() == RE::MagicSystem::Delivery::kAimed) {
		return true;
	} else {
		return false;
	}
}
