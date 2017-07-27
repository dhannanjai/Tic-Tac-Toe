#include "Field.h"
#include<assert.h>
#include<algorithm>

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

void Field::Tile::Hide()
{
	assert(!IsHidden());
	state = State::Hidden;
}

bool Field::Tile::operator==(const Tile & rhs) const
{
	return (state == rhs.state);
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

bool Field::OnClick(const Vec2i offset, const Vec2i & screenpos)
{
	Vec2i gridpos = ScreenToGrid(offset, screenpos);
	Tile& tile = TileAt(gridpos);
	
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);

	if (tile.IsHidden())
	{
		tile.Cross();
		return true;
	}
	return false;
}

RectI Field::GetRect(const Vec2i & offset) const
{
	return RectI(offset + Vec2i(0, 0), offset + Vec2i(width * SpriteCodex::tileSize, height * SpriteCodex::tileSize));
}

Field::Tile & Field::TileAt(const Vec2i & gridpos)
{
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);
	return tiles[gridpos.x][gridpos.y];
}

const Field::Tile & Field::TileAt(const Vec2i & gridpos) const
{
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);
	return tiles[gridpos.x][gridpos.y];
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

int Field::EvaluateScore() const
{
	/****along the rows****/
	for (int rows = 0; rows < width; rows++)
	{
		if (tiles[rows][0] == tiles[rows][1] && tiles[rows][1] == tiles[rows][2])
		{
			if (tiles[rows][0].IsCrossed())
				return 10;
			else if (tiles[rows][0].IsBombed())
				return -10;
		}
	}
	/****along the cols****/
	for (int cols = 0; cols < height; cols++)
	{
		if (tiles[0][cols] == tiles[1][cols] && tiles[1][cols] == tiles[2][cols])
		{
			if (tiles[0][cols].IsCrossed())
				return 10;
			else if (tiles[0][cols].IsBombed())
				return -10;
		}
	}
	/********************/
	/* in the diagnols */
	if (tiles[0][0] == tiles[1][1] && tiles[1][1] == tiles[2][2])
	{
		if (tiles[0][0].IsCrossed())
			return +10;
		else if (tiles[0][0].IsBombed())
			return -10;
	}

	if (tiles[0][2] == tiles[1][1] && tiles[1][1] == tiles[2][0])
	{
		if (tiles[0][2].IsCrossed())
			return +10;
		else if (tiles[0][2].IsBombed())
			return -10;
	}
	/*****************/
	return 0;
}

int Field::minimax(int depth, bool player)
{
	int score = EvaluateScore();

	if (player)// If this is player's move.
	{
		int best = -1000;
		for (int i = 0; i<width; i++) {					
			for (int j = 0; j<height; j++){
				if (tiles[i][j].IsHidden()){	
					tiles[i][j].Cross(); // take the condition to cross it and check if it is worth it in next statement.

					best = std::max<int>(best, minimax(depth + 1, !player)); // give the opponent(ie the AI) the chance and check the best score.
					
					tiles[i][j].Hide();// Undo the move
				}
			}
		}
		return best + score;
	}
	else// If this is AI's move
	{
		int best = 1000;
		for (int i = 0; i<width; i++){
			for (int j = 0; j<height; j++){
				if (tiles[i][j].IsHidden())
				{
					tiles[i][j].Bomb();//  take the condition to cross it and check if it is worth it in next statement.

					best = std::min<int>(best, minimax(depth + 1, !player)); // give the opponent(ie the player) the chance and check the best score.

					tiles[i][j].Hide();// Undo the move
				}
			}
		}
		return best + score;
	}
}

bool Field::HasWon() const
{
	return (EvaluateScore() > 0);
}

bool Field::HasLost() const
{
	return (EvaluateScore() < 0);
}

bool Field::IsDraw() const
{
	int count = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
			if (!tiles[i][j].IsHidden())
				count++;
	}//to check it does go on bombing somewhere and give assertion error.
	if (count < width*height)
		return false;
	return true;
}

void Field::MoveBestMove()
{
	if (HasWon() || HasLost())
		return;

	int bestVal = -1000;
	
	Vec2i bestMove = Vec2i(-1, -1);

	if (!IsDraw())
	{
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				if (tiles[i][j].IsHidden())
				{
					// Make the move
					tiles[i][j].Cross();

					// compute evaluation function for this move.
					int moveVal = minimax(0, false);

					// Undo the move
					tiles[i][j].Hide();

					// update best
					if (moveVal >= bestVal)
					{
						bestMove.x = i;
						bestMove.y = j;
						bestVal = moveVal;
					}
				}
			}
		}
		if (bestMove.x != -1 && bestMove.y != -1)
			tiles[bestMove.x][bestMove.y].Bomb();
	}
}

