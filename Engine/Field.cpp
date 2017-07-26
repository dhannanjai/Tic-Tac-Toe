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
