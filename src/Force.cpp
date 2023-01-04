#include <numbers>

#include <SimpleIni.h>

#include "Force.h"

void Magyk::Force::SetMaxVelocity() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\Magyk.ini");
	max_velocity = ini.GetDoubleValue("Global", "fMaxVelocity", 18.0f);
}

void Magyk::Force::GetFallDamage() {
	auto fall_setting = RE::GameSettingCollection::GetSingleton()->GetSetting("fJumpFallHeightMin");
	if (fall_setting) {
		fall_damage = fall_setting;
		original_fall_damage = fall_setting->GetFloat();
	}
}

void Magyk::Force::SetDefaults() {
	drag = 0.0f;
	facing_cycle = 0;
	jump_cycle = 0;
	facing_down = false;
	has_jumped = false;
	is_hovering = false;
	increasing = false;
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
			if (CheckDirection()) {
				facing_down = true;
			}
		} else {
			facing_cycle += 1;
			if (facing_cycle >= facing_window) {
				facing_cycle = 0;
				if (!CheckDirection()) {
					facing_down = false;
				}
			}
		}
		if (has_jumped) {
			if (facing_down) {
				fall_damage->data.f = 100000.0f;
				has_jumped = false;
				IncreaseElevation(a_controller, 1.5f);
				increasing = true;
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
				bool use_drag = true;
				if (increasing) {
					if (r_cast_out || l_cast_out) {
						if (!(r_cast_out && l_cast_out)) {
							if (drag > max_velocity) {
								drag += 0.075f;
							} else {
								drag += 0.25f;
							}
						} else {
							if (drag > max_velocity) {
								drag += 0.05f;
							} else {
								drag += 0.125f;
							}
						}
						if (!a_actor->IsInMidair()) {
							a_actor->InterruptCast(false);
							fall_damage->data.f = original_fall_damage;
						}
					} else {
						increasing = false;
						fall_damage->data.f = original_fall_damage;
					}
				} else {
					drag += 0.5f;
					if (drag > max_velocity) {
						use_drag = false;
					}
					if (!a_actor->IsInMidair()) {
						can_hover = false;
						//fall_damage->data.f = original_fall_damage;
					}
				}
				if (use_drag) {
					velo[2] = (max_velocity - drag);
					controller->SetLinearVelocityImpl(hkv);
					DampenFall(controller);
				}
			} else {
				CheckConditions(controller);
			}
		}
	}
}
