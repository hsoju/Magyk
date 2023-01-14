#include "Jumper.h"

void Magyk::Jumper::AddJumpSink() {
	auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
	if (deviceManager) {
		deviceManager->AddEventSink(Jumper::GetSingleton());
	}
}


uint32_t Magyk::Jumper::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
{
	uint32_t idx;
	
	switch (a_key) {
	case RE::BSWin32GamepadDevice::Key::kUp:
		idx = 0;
		break;
	case RE::BSWin32GamepadDevice::Key::kDown:
		idx = 1;
		break;
	case RE::BSWin32GamepadDevice::Key::kLeft:
		idx = 2;
		break;
	case RE::BSWin32GamepadDevice::Key::kRight:
		idx = 3;
		break;
	case RE::BSWin32GamepadDevice::Key::kStart:
		idx = 4;
		break;
	case RE::BSWin32GamepadDevice::Key::kBack:
		idx = 5;
		break;
	case RE::BSWin32GamepadDevice::Key::kLeftThumb:
		idx = 6;
		break;
	case RE::BSWin32GamepadDevice::Key::kRightThumb:
		idx = 7;
		break;
	case RE::BSWin32GamepadDevice::Key::kLeftShoulder:
		idx = 8;
		break;
	case RE::BSWin32GamepadDevice::Key::kRightShoulder:
		idx = 9;
		break;
	case RE::BSWin32GamepadDevice::Key::kA:
		idx = 10;
		break;
	case RE::BSWin32GamepadDevice::Key::kB:
		idx = 11;
		break;
	case RE::BSWin32GamepadDevice::Key::kX:
		idx = 12;
		break;
	case RE::BSWin32GamepadDevice::Key::kY:
		idx = 13;
		break;
	case RE::BSWin32GamepadDevice::Key::kLeftTrigger:
		idx = 14;
		break;
	case RE::BSWin32GamepadDevice::Key::kRightTrigger:
		idx = 15;
		break;
	default:
		idx = kInvalid;
		break;
	}

	return idx != kInvalid ? idx + kGamepadOffset : kInvalid;
}


uint32_t Magyk::Jumper::GetJumpKey(RE::INPUT_DEVICE a_device)
{
	auto key = RE::ControlMap::GetSingleton()->GetMappedKey(RE::UserEvents::GetSingleton()->jump, a_device);

	switch (a_device) {
	case RE::INPUT_DEVICE::kMouse:
		key += kMouseOffset;
		break;
	case RE::INPUT_DEVICE::kKeyboard:
		key += kKeyboardOffset;
		break;
	case RE::INPUT_DEVICE::kGamepad:
		key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
		break;
	case RE::INPUT_DEVICE::kVRLeft:
		key += kVROffset;
		break;
	case RE::INPUT_DEVICE::kVRRight:
		key += kVROffset;
		break;
	default:
		key = 255;
		break;
	}
	
	return key;
}
