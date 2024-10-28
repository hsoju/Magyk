#pragma once

#include "Force.h"

namespace Magyk
{
	class Jumper : public RE::BSTEventSink<RE::InputEvent*> {
	public:
		static Jumper* GetSingleton() {
			static Jumper singleton;
			return &singleton;
		}

		void CheckConditions() {

		}

		virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
		{
			if (!Magyk::Force::GetSingleton()->can_hover || Magyk::Force::GetSingleton()->has_jumped) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (!a_event) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto ui = RE::UI::GetSingleton();
			if (!ui || ui->GameIsPaused()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			//auto control_map = RE::ControlMap::GetSingleton();

			//if ((!control_map) || !control_map->IsMovementControlsEnabled()) {
			//	return RE::BSEventNotifyControl::kContinue;
			//}

			for (auto given_input = *a_event; given_input; given_input = given_input->next) {
				if (given_input->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) {
					continue;
				}


				const RE::ButtonEvent* button = given_input->AsButtonEvent();
				if (button->IsHeld()) {
					continue;
				}


				const RE::IDEvent* given_id = given_input->AsIDEvent();
				const auto& given_user = given_id->userEvent;
				const auto user_events = RE::UserEvents::GetSingleton();

				/*
				uint32_t current_key = 9000;
				uint32_t key_posn = 0;
				
				RE::INPUT_DEVICE     device = event->GetDevice();
				RE::INPUT_EVENT_TYPE input_type = event->GetEventType();

				
				if (input_type == RE::INPUT_EVENT_TYPE::kButton) {
					const auto button = event->AsButtonEvent();
					if (!button) {
						continue;
					} else {
						current_key = button->GetIDCode();
						if (button->IsUp()) {
							key_posn = 1;
						} else {
							if (button->IsPressed()) {
								key_posn = 2;
							} else {
								continue;
							}
						}
					}
				} else {
					continue;
				}
				
				switch (device) {
				case RE::INPUT_DEVICE::kMouse:
					current_key += kMouseOffset;
					break;
				case RE::INPUT_DEVICE::kKeyboard:
					current_key += kKeyboardOffset;
					break;
				case RE::INPUT_DEVICE::kGamepad:
					current_key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)current_key);
					break;
				case RE::INPUT_DEVICE::kVRLeft:
					current_key += kVROffset;
					break;
				case RE::INPUT_DEVICE::kVRRight:
					current_key += kVROffset;
					break;
				default:
					continue;
				}

				//const auto jump_key = GetJumpKey(device);
				const auto jump_key = 30;

				*/
				if (given_user == user_events->jump) {
					Magyk::Force::GetSingleton()->has_jumped = true;
					break;
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		void AddJumpSink();

	protected:
		Jumper() = default;
		Jumper(const Jumper&) = delete;
		Jumper(Jumper&&) = delete;
		virtual ~Jumper() = default;

		auto operator=(const Jumper&) -> Jumper& = delete;
		auto operator=(Jumper&&) -> Jumper& = delete;

		enum : uint32_t
		{
			kKeyboardOffset = 0,
			kVROffset = 0,
			kMouseOffset = 256,
			kGamepadOffset = 266,
			kInvalid = static_cast<uint32_t>(-1)
		};

	private:
		uint32_t GetJumpKey(RE::INPUT_DEVICE a_device);
		uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);

	};
}
