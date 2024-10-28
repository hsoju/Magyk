#pragma once

#include "Force.h"

namespace Magyk
{
	class Caster : public RE::BSTEventSink<RE::TESSpellCastEvent> {
	
	public:
		static Caster* GetSingleton() {
			static Caster singleton;
			return &singleton;
		}

		virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* a_event, 
			RE::BSTEventSource<RE::TESSpellCastEvent>*)
		{
			if (a_event) {
				auto event_obj = a_event->object.get();
				if (event_obj && event_obj->IsPlayerRef()) {
					auto player = event_obj->As<RE::Actor>();
					auto force = Magyk::Force::GetSingleton();
					if (!player->IsInMidair() && !force->can_hover) {
						auto spell = RE::TESForm::LookupByID(a_event->spell)->As<RE::SpellItem>();
						if (spell && IsValidSpell(spell)) {
							if (!use_blacklist || (use_blacklist && whitelist.contains(spell))) {
								force->SetDefaults();
								//if (spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget) {
								//	force->is_launched = true;
								//}
								force->can_hover = true;
								//logger::info("Hover set");
							}
						}
					}
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		bool use_blacklist = false;
		std::set<RE::SpellItem*> whitelist;

		void GetBlacklist();
		void GetWhitelist();
		void AddSpellCastSink();

	protected:
		Caster() = default;
		Caster(const Caster&) = delete;
		Caster(Caster&&) = delete;
		virtual ~Caster() = default;

		auto operator=(const Caster&) -> Caster& = delete;
		auto operator=(Caster&&) -> Caster& = delete;

	private:
		bool IsValidSpell(RE::SpellItem* a_spell);
	};
}
