#pragma once
#include"Vec2i.h"

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
	};
public:
	void OnClick(const Vec2i& screenpos);
	
	Tile& TileAt(const Vec2i& gridpos);
	const Tile& TileAt(const Vec2i& gridpos)const;

	Vec2i ScreenToGrid(const Vec2i& offset, const Vec2i screenpos);
};