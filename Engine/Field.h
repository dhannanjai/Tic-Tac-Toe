#pragma once
#include"Vec2i.h"
#include"SpriteCodex.h"
class Field
{
private :
	class Tile
	{
	private:
		enum class State
		{
			Hidden,
			Crossed,
			Bombed
		};

	private:
		State state = State::Hidden;
	public:
		bool IsHidden()const;
		bool IsCrossed()const;
		bool IsBombed()const;

		void Cross();
		void Bomb();
		void Hide();
		bool operator==(const Tile& rhs)const;

		void Draw(const Vec2i& offset, Graphics& gfx)const;
	};
public:
	bool OnClick(const Vec2i offset,const Vec2i& screenpos);
	RectI GetRect(const Vec2i& offset)const;
	Tile& TileAt(const Vec2i& gridpos);
	const Tile& TileAt(const Vec2i& gridpos)const;

	Vec2i ScreenToGrid(const Vec2i& offset, const Vec2i screenpos);

	void Draw(const Vec2i& offset,Graphics& gfx)const;
	/************** Artificial Intelligece down there  ;D **************/
private:
	int EvaluateScore()const;
	int minimax(int depth, bool isMax);
	
public:
	void MoveBestMove();
	bool HasWon()const;
	bool HasLost()const;
	bool IsDraw()const;
	/*******************************************************************/

public:
	const static int width = 3;
	const static int height = 3;
private:
	Tile tiles[width][height];
};