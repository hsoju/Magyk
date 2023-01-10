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
			if (!Magyk::Force::GetSingleton()->can_hover) {
				RE::BSEventNotifyControl::kContinue;
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

				const auto button = static_cast<RE::ButtonEvent*>(event);
				if (!button) {
					continue;
				} else {
					if (Magyk::Force::GetSingleton()->is_hovering || !button->IsUp()) {
						continue;
					}
				}

				auto current_key = button->GetIDCode();
				switch (button->device.get()) {
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

				const auto jump_key = GetJumpKey(button->device.get());

				if (current_key == jump_key) {
					if (button->IsUp()) {
						Magyk::Force::GetSingleton()->time_jumped = button->HeldDuration();
						Magyk::Force::GetSingleton()->has_jumped = true;
					}
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
