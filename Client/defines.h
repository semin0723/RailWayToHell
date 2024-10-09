#pragma once
#include <vector>
#include "Engine.h"

#define TILE_COUNT 7
#define TILE_CENTER_INDEX (TILE_COUNT / 2)

#define DEFAULT_TILE_Y (-320.0f)

class DEFINE
{
public:

	static bool IsRangeTileIndex(int tileIndex)
	{
		return !(tileIndex < 0 || tileIndex >= TILE_COUNT);
	}
	static float GetTilePositionX(int tileIndex)
	{
		if (tileIndex < 0 || tileIndex >= TILE_COUNT)
		{
			Assert(false, "tile index overrun");
		}

		int gap = tileIndex - TILE_CENTER_INDEX;
		return gap * 200.f;
	}
};



struct OrderLayer
{
	// Object
	static inline constexpr int DefaultObject = 0;

	static inline constexpr int AimShade = 50;
	static inline constexpr int GunPowder = 90;
	static inline constexpr int Enemy_Boss_BackGround = 98;
	static inline constexpr int Enemy_Boss_Body = 99;
	static inline constexpr int Player = 100;
	static inline constexpr int Enemy = 110;
	static inline constexpr int Chandelier = 120;
	static inline constexpr int Enemy_Boss_Hand = 126;
	static inline constexpr int Light = 130;


	static inline constexpr int monster_hitEffect = 140;

	static inline constexpr int hp_bar = 150;
	static inline constexpr int indicator_default = 155;
	static inline constexpr int dialog = 166;
	static inline constexpr int text = 167;
	static inline constexpr int train_spark = 168;
	static inline constexpr int warning_sign = 169;
	static inline constexpr int vfx_default = 170;

	static inline constexpr int BackGround = -100;
	static inline constexpr int SubTrain = -98;
	static inline constexpr int Train = -97;
	static inline constexpr int TrainWheel = Train - 1;
	static inline constexpr int TrainRail = TrainWheel - 1;
	static inline constexpr int TrainInside = 201; // <- ¹¹ÀÓ??
	static inline constexpr int TrainOutSide = TrainInside + 1;

	// UI
	static inline constexpr int UIDefault = 0;
	static inline constexpr int GameOver = 9999;
	static inline constexpr int GameOver_ui = 10000;


	static inline constexpr int pause_menu_background = 10000;
	static inline constexpr int pause_menu_btn = 10001;
	static inline constexpr int MouseUI = INT_MAX;
};