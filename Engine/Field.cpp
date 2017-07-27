#include "Field.h"
#include<assert.h>

bool Field::Tile::IsHidden() const
{
	return state == State::Hidden;
}

bool Field::Tile::IsCrossed() const
{
	return state == State::Crossed;
}

bool Field::Tile::IsBombed() const
{
	return state == State::Bombed;
}

void Field::Tile::Cross()
{
	assert(IsHidden());
	state = State::Crossed;
}

void Field::Tile::Bomb()
{
	assert(IsHidden());
	state = State::Bombed;
}

void Field::Tile::Draw(const Vec2i & screenpos, Graphics & gfx) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenpos, gfx);
		break;
	case State::Crossed:
		SpriteCodex::DrawTileCross(screenpos, gfx);
		break;
	case State::Bombed:
		SpriteCodex::DrawTileBomb(screenpos, gfx);
		break;
	}
}

Field::Field()
{
}

void Field::OnClick(const Vec2i offset, const Vec2i & screenpos)
{
	Vec2i gridpos = ScreenToGrid(offset, screenpos);
	Tile& tile = TileAt(gridpos);
	
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);

	if(tile.IsHidden())
		tile.Cross();
}

RectI Field::GetRect(const Vec2i & offset) const
{
	return RectI(offset + Vec2i(0, 0), offset + Vec2i(width * SpriteCodex::tileSize, height * SpriteCodex::tileSize));
}

Field::Tile & Field::TileAt(const Vec2i & gridpos)
{
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);
	return tiles[gridpos.x + gridpos.y * width];
}

const Field::Tile & Field::TileAt(const Vec2i & gridpos) const
{
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);
	return tiles[gridpos.x + gridpos.y * width];
}

Vec2i Field::ScreenToGrid(const Vec2i & offset, const Vec2i screenpos)
{
	return (screenpos - offset)/SpriteCodex::tileSize;
}

void Field::Draw(const Vec2i & offset, Graphics & gfx) const
{
	RectI rect = GetRect(offset);
	gfx.DrawRect(rect, SpriteCodex::baseColor);

	Vec2i gridpos = Vec2i(0, 0);
	for (; gridpos.x < width; gridpos.x++)
	{
		for (gridpos.y = 0; gridpos.y < height; gridpos.y++)
		{
			TileAt(gridpos).Draw(offset + gridpos * SpriteCodex::tileSize, gfx);
		}
	}
}
