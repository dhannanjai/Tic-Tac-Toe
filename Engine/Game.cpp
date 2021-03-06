/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include"Graphics.h"


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	offset(Graphics::GetScreenRect().GetCenter() - Vec2i((Field::width * SpriteCodex::tileSize) / 2, (Field::height * SpriteCodex::tileSize) / 2))
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	bool isClicked = false;
	/*
	if (setTheField == true)
	{
		field.tile[2][0].Cross();
		field.tile[0][2].Cross();
		field.tile[1][1].Bomb();
		field.tile[2][1].Bomb();
		field.tile[2][2].Cross();
		isClicked = true;
		setTheField = false;
	}*/
	if (!field.IsDraw() && !field.HasWon() && !field.HasLost())
	{
		while (!wnd.mouse.IsEmpty())
		{
			const auto e = wnd.mouse.Read();
			if (e.GetType() == Mouse::Event::Type::LPress)
			{
				Vec2i screenPos = wnd.mouse.GetPos();
				if (field.GetRect(offset).Contains(screenPos))
				{
					isClicked = field.OnClick(offset, screenPos);
				}
			}
		}
	}
	if (isClicked)
	{
		isClicked = false;
		field.MoveBestMove();
	}
}

void Game::ComposeFrame()
{
	field.Draw(offset, gfx);
}
