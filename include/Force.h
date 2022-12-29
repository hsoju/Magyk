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

		void Update(RE::Actor* a_actor);

		static inline bool      floating = false;
		static inline bool		hovering = false;
		static inline bool		increasing = false;

		static inline bool		r_cast_out = false;
		static inline bool		l_cast_out = false;

		const RE::BSFixedString r_cast = RE::BSFixedString("bWantCastRight");
		const RE::BSFixedString l_cast = RE::BSFixedString("bWantCastLeft");
 
		static uint32_t RadianRange(float a_radian);
		static float RadiansToDegrees(float a_radian);
		static bool CheckDirection(bool use_axis=false);

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
