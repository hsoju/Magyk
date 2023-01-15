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

		auto ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) -> RE::BSEventNotifyControl override {
			if (!Magyk::Force::GetSingleton()->can_hover || Magyk::Force::GetSingleton()->has_jumped) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (!a_event) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (RE::UI::GetSingleton()->GameIsPaused() || !RE::ControlMap::GetSingleton()->IsMovementControlsEnabled()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			for (auto event = *a_event; event; event = event->next) {
				if (event->eventType != RE::INPUT_EVENT_TYPE::kButton) {
					continue;
				}

				uint32_t current_key = 9000;
				uint32_t key_posn = 0;
				
				RE::INPUT_DEVICE     device = event->GetDevice();
				RE::INPUT_EVENT_TYPE input_type = event->GetEventType();

				if (input_type == RE::INPUT_EVENT_TYPE::kButton) {
					const auto button = static_cast<RE::ButtonEvent*>(event);
					if (!button) {
						continue;
					} else {
						current_key = button->GetIDCode();
						if (button->IsUp()) {
							key_posn = 1;
						} else {
							//continue;
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

				const auto jump_key = GetJumpKey(device);

				if (current_key == jump_key) {
					//logger::info("Keys matched, Key state = {}", key_posn);
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
