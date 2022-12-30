#include <numbers>
#include "Force.h"

uint32_t Magyk::Force::RadianRange(float a_radian) {
	if (a_radian < (std::numbers::pi / 2.0)) {
		return 0;
	} else {
		if (a_radian < std::numbers::pi) {
			return 1;
		} else {
			if (a_radian < (std::numbers::pi * 1.5)) {
				return 2;
			} else {
				if (a_radian < (std::numbers::pi * 2.0)) {
					return 3;
				} else {
					return 4;
				}
			}
		}
	}
}

float Magyk::Force::RadiansToDegrees(float a_radian) {
	return a_radian * 180.0f / std::numbers::pi;
}

bool Magyk::Force::CheckDirection(bool use_axis) {
	auto cam = RE::PlayerCamera::GetSingleton();
	if (cam) {
		auto rot = cam->cameraRoot->local.rotate;
		float x;
		float y;
		float z;
		rot.ToEulerAnglesXYZ(x, y, z);
		x = RadiansToDegrees(x);
		y = RadiansToDegrees(y);
		if (abs(x) > 40.0f || abs(y) > 30.0f) {
			if (use_axis) {
				uint32_t axis = RadianRange(cam->yaw);
				if ((x + y) > 0.0f) {
					if (axis == 0 || axis == 3) {
						return true;
					}
				} else {
					if (axis == 1 || axis == 2) {
						return true;
					}
				}
			} else {
				return true;
			}
		}
	}
	return false;
}

void Magyk::Force::IncreaseElevation(RE::bhkCharacterController* a_controller, float height) {
	RE::hkVector4 hkp;
	a_controller->GetPositionImpl(hkp, false);
	auto posn = hkp.quad.m128_f32;
	posn[2] += height;
	a_controller->SetPositionImpl(hkp, false, false);
}

void Magyk::Force::DampenFall(RE::bhkCharacterController* a_controller) {
	RE::hkVector4 hkp;
	a_controller->GetPositionImpl(hkp, false);
	auto posn = hkp.quad.m128_f32;
	a_controller->fallStartHeight = posn[2];
	a_controller->fallTime = 0.0f;
}

void Magyk::Force::Update(RE::Actor* a_actor) {
	if (a_actor->Is3DLoaded()) {
		if (floating) {
			a_actor->GetGraphVariableBool(r_cast, r_cast_out);
			a_actor->GetGraphVariableBool(l_cast, l_cast_out);
			auto controller = a_actor->GetCharController();
			if (hovering) {
				RE::hkVector4 hkv;
				controller->GetLinearVelocityImpl(hkv);
				auto velo = hkv.quad.m128_f32;
				if (increasing) {
					if (r_cast_out || l_cast_out) {
						lift += 0.25f;
					} else {
						increasing = false;
					}
				} else {
					lift += 0.5f;
					if (lift > max_height) {
						floating = false;
					}
				}
				velo[2] = (max_height - lift);
				controller->SetLinearVelocityImpl(hkv);
				DampenFall(controller);
			} else {
				if (r_cast_out || l_cast_out) {
					if (CheckDirection(true)) {
						IncreaseElevation(controller, 1.0f);
						hovering = true;
					}
				} else {
					floating = false;
				}
			}
		}
	}
}
