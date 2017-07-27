#pragma once

#include "Graphics.h"
#include "Vec2i.h"

class SpriteCodex
{
public:
	// width and height of all tiles
	static constexpr int tileSize = 16;
	// base color for all tiles
	static constexpr Color baseColor = { 192,192,192 };
	// 16x16 tile sprites assume (192,192,192) background and top left origin
	
	static void DrawTileButton(const Vec2i& pos, Graphics& gfx);
	static void DrawTileCross(const Vec2i& pos, Graphics& gfx);
	static void DrawTileBomb(const Vec2i& pos, Graphics& gfx);
	static void DrawTileBombRed(const Vec2i& pos, Graphics& gfx);
	// Tile selector function valid input 0-8
	// Win Screen 254x192 center origin
	static void DrawWin(const Vec2i& pos, Graphics& gfx);

};