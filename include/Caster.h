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
				if (!player->IsInMidair() && Magyk::Force::CheckDirection(true)) {
					auto spell = RE::TESForm::LookupByID(a_event->spell)->As<RE::SpellItem>();
					if (IsValidSpell(spell)) {
						RE::hkVector4 hkp;
						auto controller = player->GetCharController();
						controller->GetPositionImpl(hkp, false);
						auto posn = hkp.quad.m128_f32;
						posn[2] += 1.0f;
						controller->SetPositionImpl(hkp, false, false);
						Magyk::Force::GetSingleton()->floating = true;
						Magyk::Force::GetSingleton()->hovering = true;
						Magyk::Force::GetSingleton()->increasing = true;
					}
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		static void AddSpellCastSink();
		static bool IsValidSpell(RE::SpellItem* a_spell);

	protected:
		Caster() = default;
		Caster(const Caster&) = delete;
		Caster(Caster&&) = delete;
		virtual ~Caster() = default;

		auto operator=(const Caster&) -> Caster& = delete;
		auto operator=(Caster&&) -> Caster& = delete;
	};
}
