#include <chrono>
#include <thread>
#include <numbers>

#include <SimpleIni.h>

#include "Force.h"

using namespace std::chrono_literals;

void Magyk::Force::SetConfig() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\Magyk.ini");
	max_velocity_z = ini.GetDoubleValue("Global", "fMaxVelocity", 18.0f);
	has_fall_damage = !(ini.GetBoolValue("Global", "bNoFallDamage", true));
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
	x_mod = 0.0f;
	y_mod = 0.0f;
	facing_cycle = 0;
	jump_cycle = 0;
	facing_down = false;
	has_jumped = false;
	time_jumped = 0.0f;
	is_launched = false;
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
	logger::info("Camera direction check");
	auto cam = RE::PlayerCamera::GetSingleton();
	if (cam) {
		auto rot = cam->cameraRoot->local.rotate;
		float x;
		float y;
		float z;
		rot.ToEulerAnglesXYZ(x, y, z);
		float dx = RadiansToDegrees(x);
		float dy = RadiansToDegrees(y);
		/* KEEP THIS HERE NO MATTER WHAT */
		//float yaw = atan(rot.entry[1][0] / rot.entry[0][0]);
		//float pitch = atan(-rot.entry[2][0] / (sqrt(pow(rot.entry[2][1], 2) + pow(rot.entry[2][2], 2))));
		float roll = atan(rot.entry[2][1] / rot.entry[2][2]);
		if (abs(dx) > 40.0f || abs(dy) > 30.0f) {
			if (use_axis) {
				logger::info("Correct camera axis");
				return roll < 0.0f;
			} else {
				return true;
			}
		} else {
			logger::info("Invalid camera-x: {}, Invalid camera-y: {}", abs(dx), abs(dy));
		}
	} else {
		logger::info("No camera found");
	}
	return true;
}

void Magyk::Force::IncreaseElevation(RE::bhkCharacterController* a_controller, float height) {
	logger::info("Has elevated");
	RE::hkVector4 hkp;
	a_controller->GetPositionImpl(hkp, false);
	auto posn = hkp.quad.m128_f32;
	float orig = posn[2];
	for (float increment = 0.1; increment <= height; increment += 0.1) {
		posn[2] = orig + increment;
		a_controller->SetPositionImpl(hkp, false, true);
	}
}

void Magyk::Force::DampenFall(RE::bhkCharacterController* a_controller) {
	RE::hkVector4 hkp;
	a_controller->GetPositionImpl(hkp, false);
	auto posn = hkp.quad.m128_f32;
	a_controller->fallStartHeight = posn[2];
	a_controller->fallTime = 0.0f;
}

void Magyk::Force::CheckView() {
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
}

void Magyk::Force::CheckJump(RE::bhkCharacterController* a_controller) {
	if (has_jumped) {
		if (facing_down) {
			logger::info("Levitation started");
			fall_damage->data.f = 100000.0f;
			has_jumped = false;
			IncreaseElevation(a_controller, 1.5f);
			increasing = true;
			is_hovering = true;
		} else {
			jump_cycle += 1;
			if (jump_cycle >= jump_window) {
				logger::info("Overshot jump window");
				jump_cycle = 0;
				has_jumped = false;
			}
		}
	}
}

void Magyk::Force::CheckLaunch(RE::bhkCharacterController* a_controller) {
	if (has_jumped) {
		fall_damage->data.f = 100000.0f;
		IncreaseElevation(a_controller, 1.5f);
		increasing = true;
		is_hovering = true;
	} else {
		jump_cycle += 1;
	}
}

void Magyk::Force::CheckConditions(RE::bhkCharacterController* a_controller) {
	if (!is_launched) {
		if (r_cast_out || l_cast_out) {
			CheckView();
			CheckJump(a_controller);
		} else {
			logger::info("Hover ended");
			can_hover = false;
		}
	} else {
		if (jump_cycle < (jump_window * 2.5)) {
			CheckLaunch(a_controller);
		} else {
			can_hover = false;
		}
	}
}

void Magyk::Force::UpdateHover(RE::Actor* a_actor) {
	logger::info("Hover update");
	if (!is_launched) {
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
						if (drag > max_velocity_z) {
							drag += 0.075f;
						} else {
							drag += 0.25f;
						}
					} else {
						if (drag > max_velocity_z) {
							drag += 0.05f;
						} else {
							drag += 0.125f;
						}
					}
					if (!a_actor->IsInMidair()) {
						can_hover = false;
						a_actor->InterruptCast(false);
						fall_damage->data.f = original_fall_damage;
					}
				} else {
					increasing = false;
					if (has_fall_damage) {
						fall_damage->data.f = original_fall_damage;
					}
				}
				DampenFall(controller);
			} else {
				if (drag < max_velocity_z) {
					drag += 0.5f;
				} else {
					use_drag = false;
				}
				if (!a_actor->IsInMidair()) {
					can_hover = false;
					fall_damage->data.f = original_fall_damage;
				}
			}
			if (use_drag) {
				velo[2] = max_velocity_z - drag;
				controller->SetLinearVelocityImpl(hkv);
			}
		} else {
			CheckConditions(controller);
		}
	} else {
		auto controller = a_actor->GetCharController();
		if (is_hovering) {
			RE::hkVector4 hkv;
			controller->GetLinearVelocityImpl(hkv);
			auto velo = hkv.quad.m128_f32;
			if (increasing) {
				if (has_jumped && (drag < max_velocity_xy || time_jumped > 0.0f)) {
					if (drag < max_velocity_xy) {
						time_jumped -= 0.005;
					}
					drag += 0.5f;
					if (!a_actor->IsInMidair()) {
						can_hover = false;
						fall_damage->data.f = original_fall_damage;
					}
				} else {
					increasing = false;
				}
				DampenFall(controller);
			} else {
				drag += 0.25f;
				if (!a_actor->IsInMidair()) {
					can_hover = false;
					fall_damage->data.f = original_fall_damage;
				}
			}
			auto cam = RE::PlayerCamera::GetSingleton();
			if (cam) {
				x_mod = sin(cam->yaw) * -1.0f;
				y_mod = cos(cam->yaw) * -1.0f;
			}
			velo[0] = drag * 1.75f * x_mod;
			velo[1] = drag * 1.75f * y_mod;
			velo[2] = max_velocity_xy - drag;
			controller->SetLinearVelocityImpl(hkv);
		} else {
			CheckConditions(controller);
		}
	}

}

void Magyk::Force::Update(RE::Actor* a_actor) {
	if (can_hover) {
		UpdateHover(a_actor);
	}
}
