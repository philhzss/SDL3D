//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SDL.h>

#include <map>
#include <cstddef> // For std::size_t
#include <vector>
#include <glm/glm.hpp>

class InputManager
{
private:
	using sdlKeyMap = std::map<int, bool>; // Using a non-const key seems to be more compatible with different implementations
	using sdlKeyMapPair = std::pair<int, bool>;

	sdlKeyMap mKeys;
	SDL_MouseMotionEvent mMouseMotionEvent;


public:
	using keyVector = std::vector<int>;
	

	InputManager();
	~InputManager();
	void registerKey(int sdlKey);
	void registerKeys(const keyVector& keys);
	void initMousePos();
	bool isKeyPressed(int sdlKey);
	glm::vec2 mMousePos;
	glm::vec2 mMouseRelPos;
	glm::vec2 getMousePos();
	glm::vec2 getMouseRelPos();


	void updateKeyByEvent(SDL_Event event);
	void updateMouseMovement(SDL_Event event); // Returns 1 if mouse moved 
};

#endif /* INPUT_MANAGER_HPP */