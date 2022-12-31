#pragma once

namespace Magyk
{
	class Force	{

	public:
		static void InstallHooks()
		{
			Hooks::Install();
		}

		static Force* GetSingleton() {
			static Force forceInterface;
			return &forceInterface;
		}

		static inline bool can_hover = false;
		static inline bool is_hovering = false;
		static inline bool increasing = false;

		static inline bool facing_down = false;
		static inline bool has_jumped = false;

		static inline uint32_t jump_cycle = 0;
		static inline uint32_t jump_window = 25;

		static inline uint32_t facing_cycle = 0;
		static inline uint32_t facing_window = 50;

		static inline float max_height = 18.0f;
		static inline float	drag = 0.0f;

		static inline bool r_cast_out = false;
		static inline bool l_cast_out = false;

		const RE::BSFixedString r_cast = RE::BSFixedString("bWantCastRight");
		const RE::BSFixedString l_cast = RE::BSFixedString("bWantCastLeft");
 
		static void SetMaxHeight();

		static uint32_t RadianRange(float a_radian);
		static float RadiansToDegrees(float a_radian);
		static bool CheckDirection(bool use_axis=false);

		static void IncreaseElevation(RE::bhkCharacterController* a_controller, float height);
		static void DampenFall(RE::bhkCharacterController* a_controller);

		void CheckConditions(RE::bhkCharacterController* a_controller);
		void Update(RE::Actor* a_actor);

	protected:
		struct Hooks
		{
			struct PlayerCharacter_Update
			{
				static void thunk(RE::PlayerCharacter* a_player) {
					func(a_player);
					GetSingleton()->Update(a_player);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			static void Install() {
				stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
			}
		};
	};
}
