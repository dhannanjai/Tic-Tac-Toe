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

int Field::evaluate() const
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

bool Field::IsMovesLeft() const
{
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				if (tiles[i][j].IsHidden())
					return true;
		return false;
}

int Field::minimax(int depth, bool isMax)
{
	int score = evaluate();

	// If Maximizer has won the game return his/her
	// evaluated score
	if (score == 10)
		return score;

	// If Minimizer has won the game return his/her
	// evaluated score
	if (score == -10)
		return score;

	// If there are no more moves and no winner then
	// it is a tie
	if (IsMovesLeft() == false)
		return 0;

	if (isMax)// If this maximizer's move
	{
		int best = -1000;
		for (int i = 0; i<width; i++) // Traverse all cells
		{
			for (int j = 0; j<height; j++)
			{
				if (tiles[i][j].IsHidden())// Check if cell is empty
				{
					tiles[i][j].Cross();// Make the move
										 // Call minimax recursively and choose the maximum value
					best = std::max<int>(best, minimax(depth + 1, !isMax));
					tiles[i][j].Hide();// Undo the move
				}
			}
		}
		return best;
	}
	else// If this minimizer's move
	{
		int best = 1000;
		for (int i = 0; i<width; i++)// Traverse all cells
		{
			for (int j = 0; j<height; j++)
			{
				if (tiles[i][j].IsHidden())// Check if cell is empty
				{

					tiles[i][j].Bomb();// Make the move
					best = std::min<int>(best, minimax(depth + 1, !isMax));
					// Call minimax recursively and choose the minimum value

					tiles[i][j].Hide();// Undo the move
				}
			}
		}
		return best;
	}
}

void Field::MoveBestMove()
{
	int bestVal = -1000;
	
	Vec2i bestMove = Vec2i(-1, -1);
	
	int count = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
			if (!tiles[i][j].IsHidden())
				count++;
	}
	if (count < width*height)
	{
		// Traverse all cells, evalutae minimax function for all empty cells. And return the cell with optimal value.
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				// Check if celll is empty
				if (tiles[i][j].IsHidden())
				{
					// Make the move
					tiles[i][j].Cross();

					// compute evaluation function for this move.
					int moveVal = minimax(0, false);

					// Undo the move
					tiles[i][j].Hide();

					// If the value of the current move is more than the best value, then update best
					if (moveVal > bestVal)
					{
						bestMove.x = i;
						bestMove.y = j;
						bestVal = moveVal;
					}
				}
			}
		}

		tiles[bestMove.x][bestMove.y].Bomb();
	}
}

