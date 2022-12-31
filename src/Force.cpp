#include <numbers>

#include <SimpleIni.h>

#include "Force.h"

void Magyk::Force::SetMaxHeight() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\Magyk.ini");
	max_height = ini.GetDoubleValue("Global", "fMaxHeight", 18.0f);
}


uint32_t Magyk::Force::RadianRange(float a_degree) {
	if (a_degree < 90.0f) {
		return 0;
	} else {
		if (a_degree < 180.0f) {
			return 1;
		} else {
			if (a_degree < 270.0f) {
				return 2;
			} else {
				if (a_degree < 360.0f) {
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
		z = RadiansToDegrees(z);
		if (abs(x) > 40.0f || abs(y) > 30.0f) {
			if (use_axis) {
				uint32_t axis = RadianRange(RadiansToDegrees(cam->yaw));
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

void Magyk::Force::CheckConditions(RE::bhkCharacterController* a_controller) {
	if (r_cast_out || l_cast_out) {
		if (!facing_down) {
			if (CheckDirection(true)) {
				facing_down = true;
			}
		} else {
			facing_cycle += 1;
			if (facing_cycle >= facing_window) {
				facing_cycle = 0;
				if (!CheckDirection(true)) {
					facing_down = false;
				}
			}
		}
		if (has_jumped) {
			if (facing_down) {
				IncreaseElevation(a_controller, 1.5f);
				is_hovering = true;
			} else {
				jump_cycle += 1;
				if (jump_cycle >= jump_window) {
					jump_cycle = 0;
					has_jumped = false;
				}
			}
		}
	} else {
		can_hover = false;
	}
}

void Magyk::Force::Update(RE::Actor* a_actor) {
	if (a_actor->Is3DLoaded()) {
		if (can_hover) {
			a_actor->GetGraphVariableBool(r_cast, r_cast_out);
			a_actor->GetGraphVariableBool(l_cast, l_cast_out);
			auto controller = a_actor->GetCharController();
			if (is_hovering) {
				RE::hkVector4 hkv;
				controller->GetLinearVelocityImpl(hkv);
				auto velo = hkv.quad.m128_f32;
				if (increasing) {
					if (r_cast_out || l_cast_out) {
						if (!(r_cast_out && l_cast_out)) {
							if (drag > max_height) {
								drag += 0.075f;
							} else {
								drag += 0.25f;
							}
						} else {
							if (drag > max_height) {
								drag += 0.05f;
							} else {
								drag += 0.125f;
							}
						}
					} else {
						increasing = false;
					}
				} else {
					drag += 0.5f;
					if (drag > max_height) {
						can_hover = false;
					}
				}
				velo[2] = (max_height - drag);
				controller->SetLinearVelocityImpl(hkv);
				DampenFall(controller);
			} else {
				CheckConditions(controller);
			}
		}
	}
}
