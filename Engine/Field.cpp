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
		SpriteCodex::DrawTileHidden(screenpos, gfx);
		break;
	case State::Crossed:
		SpriteCodex::DrawTilePress(screenpos, gfx);
		break;
	case State::Bombed:
		SpriteCodex::DrawTileAI(screenpos, gfx);
		break;
	}
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
	return tile[gridpos.x][gridpos.y];
}

const Field::Tile & Field::TileAt(const Vec2i & gridpos) const
{
	assert(gridpos.x >= 0 && gridpos.y < width && gridpos.y >= 0 && gridpos.y < height);
	return tile[gridpos.x][gridpos.y];
}

Vec2i Field::ScreenToGrid(const Vec2i & offset, const Vec2i screenpos)
{
	return (screenpos - offset)/SpriteCodex::tileSize;
}

void Field::Draw(const Vec2i & offset, Graphics & gfx) const
{
	Vec2i gridpos = Vec2i(0, 0);
	for (; gridpos.x < width; gridpos.x++)
	{
		for (gridpos.y = 0; gridpos.y < height; gridpos.y++)
		{
			TileAt(gridpos).Draw(offset + gridpos * SpriteCodex::tileSize, gfx);
		}
	}
}

bool Field::HasWon() const
{
	return (EvaluateState() > 0);
}

bool Field::HasLost() const
{
	return (EvaluateState() < 0);
}

bool Field::IsDraw() const
{
	int count = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
			if (!tile[i][j].IsHidden())
				count++;
	}//to check it does go on bombing somewhere and give assertion error.
	if (count < width*height)
		return false;
	return true;
}

int Field::EvaluateLine(int a, int b, int m, int n, int x, int y) const
{
	int score = 0;

	// first tile 
	if (tile[a][b].IsBombed())			// the tile[a][b] belongs to AI
		score = 1;
	else if (tile[a][b].IsCrossed())	// the tile[a][b] belongs to player
		score = -1;

	//second tile
	if (tile[m][n].IsBombed())			// the tile[m][n] belongs to AI
	{
		if (score == 1)					// 2- in a line
			score = 10;
		else if (score == -1)			// 1- in a  line
			return 0;
		else							// tile[a][b] is not occupied yet but tile[m][n] is
			score = 1;
	}
	else if (tile[m][n].IsCrossed())	// if tile[m][n] is Crossed
	{
		if (score == -1)					// 2- in a line 
			score = -10;
		else if (score == 1)			// 1- in a line
			return 0;
		else							// tile[a][b] is not occupied yet but tile[m][n] is
			score = -1;
	}

	//third tile
	if (tile[x][y].IsBombed())			// if tile[x][y] is bombed.
	{
		if (score > 0)					//means either 2- or 1- in a row for AI.
			score *= 10;
		else if (score < 0)				// means either 2- or 1- in a row for player.
			return 0;
		else							// means tile[a][b] and tile[m][n] is unoccupied.
			score = 1;
	}
	else if (tile[x][y].IsCrossed())	// if tile[x][y] is crossed
	{
		if (score < 0)					// means either 2- or 1- in a row for player.
			score *= 10;
		else if (score > 0)				//means either 2 - or 1 - in a row for AI.
			return 0;
		else							//means tile[a][b] and tile[m][n] is unoccupied.
			score = -1;
	}

	return score;
}

int Field::EvaluateState() const
{
	/****along the rows****/
	for (int rows = 0; rows < width; rows++)
	{
		if (tile[rows][0] == tile[rows][1] && tile[rows][1] == tile[rows][2])
		{
			if (tile[rows][0].IsCrossed())
				return 10;
			else if (tile[rows][0].IsBombed())
				return -10;
		}
	}
	/****along the cols****/
	for (int cols = 0; cols < height; cols++)
	{
		if (tile[0][cols] == tile[1][cols] && tile[1][cols] == tile[2][cols])
		{
			if (tile[0][cols].IsCrossed())
				return 10;
			else if (tile[0][cols].IsBombed())
				return -10;
		}
	}
	/********************/
	/* in the diagnols */
	if (tile[0][0] == tile[1][1] && tile[1][1] == tile[2][2])
	{
		if (tile[0][0].IsCrossed())
			return +10;
		else if (tile[0][0].IsBombed())
			return -10;
	}

	if (tile[0][2] == tile[1][1] && tile[1][1] == tile[2][0])
	{
		if (tile[0][2].IsCrossed())
			return +10;
		else if (tile[0][2].IsBombed())
			return -10;
	}
	/*****************/
	return 0;
}

int Field::EvaluateScore() const
{
	int score = 0;
	score += EvaluateLine(0, 0, 0, 1, 0, 2);	// for col 0
	score += EvaluateLine(1, 0, 1, 1, 1, 2);	// for col 1
	score += EvaluateLine(2, 0, 2, 1, 2, 2);	// for col 2
	score += EvaluateLine(0, 0, 1, 0, 2, 0);	// for row 0
	score += EvaluateLine(0, 1, 1, 1, 2, 1);	// for row 1
	score += EvaluateLine(0, 2, 1, 2, 2, 2);	// for row 2
	score += EvaluateLine(0, 0, 1, 1, 2, 2);	// for diagnol 0
	score += EvaluateLine(0, 2, 1, 1, 2, 0);	// for diagnol 1
	return score;
}

int Field::minimax(bool player)
{
	//first, evaluate the current score of the position of the board.
	int score = EvaluateScore();
	if (HasWon() || HasLost() || IsDraw())
		return score;

	if (player)    //player's turn
	{
		int bestValue = INT_MAX; // because it will never reach that value.

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
			{
				if (tile[i][j].IsHidden())
				{
					tile[i][j].Cross();

					bestValue = std::min<int>(minimax(!player), bestValue);

					tile[i][j].Hide();
				}
			}
		return bestValue + score;
	}

	else		// AI turn.
	{
		int bestValue = INT_MIN;

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
			{
				if (tile[i][j].IsHidden())
				{
					tile[i][j].Bomb();

					bestValue = std::max<int>(minimax(!player), bestValue);

					tile[i][j].Hide();
				}
			}
		return bestValue + score;
	}
}

void Field::MoveBestMove()
{
	if (!HasWon() && !HasLost() && !IsDraw())
	{
		Vec2i bestMove = { -1,-1 };
		int bestValue = INT_MIN;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				if (tile[i][j].IsHidden())
				{
					tile[i][j].Bomb();

					int value = minimax(true);		//give the player the turn.

					tile[i][j].Hide();

					if (value > bestValue)
					{
						bestValue = value;
						bestMove = { i,j };
					}
				}
			}

		tile[bestMove.x][bestMove.y].Bomb();
	}
}
