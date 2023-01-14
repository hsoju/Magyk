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

		auto ProcessEvent(const RE::TESSpellCastEvent* a_event, RE::BSTEventSource<RE::TESSpellCastEvent>* a_eventSource) -> RE::BSEventNotifyControl override {
			if (a_event && a_event->object && a_event->object->IsPlayerRef()) {
				auto player = a_event->object->As<RE::Actor>();
				if (!player->IsInMidair() && !Magyk::Force::GetSingleton()->can_hover) {
					auto spell = RE::TESForm::LookupByID(a_event->spell)->As<RE::SpellItem>();
					if (spell && IsValidSpell(spell)) {
						if (!use_blacklist || (use_blacklist && whitelist.contains(spell))) {
							auto force = Magyk::Force::GetSingleton();
							force->SetDefaults();
							//if (spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget) {
							//	force->is_launched = true;
							//}
							force->can_hover = true;
							logger::info("Hover started");
							//force->UpdateHover(RE::PlayerCharacter::GetSingleton()->As<RE::Actor>());
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
