#include "Caster.h"

void Magyk::Caster::AddSpellCastSink() {
	auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
	if (sourceHolder) {
		sourceHolder->AddEventSink(Caster::GetSingleton());
	}
}
