#pragma once

namespace Magyk
{
	class Force	{

	public:
		static void Register()
		{
			Hooks::Install();
		}

		static Force* GetSingleton() {
			static Force forceInterface;
			return &forceInterface;
		}

		bool can_hover = false;
		bool is_hovering = false;
		bool increasing = false;
		bool is_launched = false;

		bool facing_down = false;
		bool has_jumped = false;
		float time_jumped = 0.0f;

		uint32_t jump_cycle = 0;
		uint32_t jump_window = 10;

		uint32_t facing_cycle = 0;
		uint32_t facing_window = 50;

		float max_velocity_xy = 10.0f;
		float max_velocity_z = 18.0f;

		float drag = 0.0f;
		float x_mod = 0.0f;
		float y_mod = 0.0f;

		bool r_cast_out = false;
		bool l_cast_out = false;

		RE::Setting* fall_damage;
		float original_fall_damage;
		bool has_fall_damage = false;

		const RE::BSFixedString r_cast = RE::BSFixedString("bWantCastRight");
		const RE::BSFixedString l_cast = RE::BSFixedString("bWantCastLeft");
 
		void SetConfig();
		void GetFallDamage();
		void SetDefaults();
		void Update(RE::Actor* a_actor);

	protected:
		struct Hooks
		{
			struct PlayerCharacter_Update
			{
				static void thunk(RE::PlayerCharacter* a_player, float a_delta) {
					func(a_player, a_delta);
					GetSingleton()->Update(a_player);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			static void Install() {
				stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
			}
		};

	private:
		uint32_t RadianRange(float a_radian);
		bool     CheckDirection(bool use_axis = false);

		void IncreaseElevation(RE::bhkCharacterController* a_controller, float height);
		void DampenFall(RE::bhkCharacterController* a_controller);

		void CheckView();
		void CheckJump(RE::bhkCharacterController* a_controller);
		void CheckLaunch(RE::bhkCharacterController* a_controller);
		void CheckConditions(RE::bhkCharacterController* a_controller);

		void UpdateHover(RE::Actor* a_actor);
		void UpdateLaunch(RE::Actor* a_actor);
	};
}
