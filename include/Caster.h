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
				if (!player->IsInMidair()) {
					RE::hkVector4 hkp;
					auto controller = player->GetCharController();
					controller->GetPositionImpl(hkp, false);
					auto posn = hkp.quad.m128_f32;
					posn[2] += 1.0f;
					controller->SetPositionImpl(hkp, false, false);
					Magyk::Force::GetSingleton()->hovering = true;
					Magyk::Force::GetSingleton()->increasing = true;
				}
				//controller->SetLinearVelocityImpl(hkv);
				//logger::info("Posn: x - {}, y - {}, z - {}, w - {}", matrx[0], matrx[1], matrx[2], matrx[3]);
				//logger::info("Velocity: x - {}, y - {}, z - {}", velo[0], velo[1], velo[2]);
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		static void AddSpellCastSink();

	protected:
		Caster() = default;
		Caster(const Caster&) = delete;
		Caster(Caster&&) = delete;
		virtual ~Caster() = default;

		auto operator=(const Caster&) -> Caster& = delete;
		auto operator=(Caster&&) -> Caster& = delete;
	};
}
