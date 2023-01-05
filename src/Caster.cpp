#include <SimpleIni.h>
#include "Caster.h"

void Magyk::Caster::GetBlacklist() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\Magyk.ini");
	use_blacklist = ini.GetBoolValue("Global", "bUseBlacklist", false);
}

void Magyk::Caster::GetWhitelist() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\Magyk.ini");
	std::list<CSimpleIniA::Entry> keys;
	ini.GetAllKeys("Whitelist", keys);
	for (auto& entry : keys) {
		bool spell_found = false;
		auto full_spell = ini.GetValue("Whitelist", entry.pItem, "");
		if (strcmp(full_spell, "") != 0) {
			const char* idx = full_spell;
			idx = strchr(idx, '|');
			if (idx != nullptr) {
				auto form_id = std::string_view(full_spell + 1, strlen(full_spell) - strlen(idx) - 1);
				auto mod_name = std::string_view(idx + 1, strlen(idx) - 2);
				char* ix;
				auto spell_id = (RE::FormID)strtoul(form_id.data(), &ix, 16);
				if (spell_id != 0) {
					auto spell = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(spell_id, mod_name);
					if (spell) {
						spell_found = true;
						whitelist.insert(spell);
					}
				}
			}
		}
		if (!spell_found) {
			logger::error("Invalid: {}", entry.pItem);
		}
	}
}

bool Magyk::Caster::IsValidSpell(RE::SpellItem* a_spell) {
	if (a_spell->GetDelivery() == RE::MagicSystem::Delivery::kAimed && a_spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration) {
		return true;
	} else {
		return false;
	}
}

void Magyk::Caster::AddSpellCastSink()
{
	auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
	if (sourceHolder) {
		sourceHolder->AddEventSink(Caster::GetSingleton());
	}
}
