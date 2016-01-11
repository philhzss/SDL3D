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

// The whole interface is defined here! This class has A LOT of dependencies!

// This class is a single script. A script can be multiple files, but this is Lua's job.
// Happily, it looks like splitting the lua states per script, like here, is a good idea.
// http://lua-users.org/lists/lua-l/2003-11/msg00011.html

#include <Script.hpp>
#include <Utils.hpp>
#include <Game.hpp>

#include <exception> // For handling the script's exceptions
#include <memory> // For smart pointers

using namespace LuaIntf; // Will make things way clearer

// mainFilePath is the script's entry point file
Script::Script(const std::string& name, const std::string& mainFilePath, const std::string& absoluteRequirePath)
{
	mName = name;
	mMainFilePath = mainFilePath;
	mMainFileContents = Utils::getFileContents(mainFilePath);
	setLuaRequirePath(absoluteRequirePath);

	try
	{
		LuaState luaState = mLuaContext.state();
		luaState.openLibs(); // Open all of Lua's basic libs, like io and math.
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Lua failed to load libraries. Error: ";
		Utils::CRASH(errorMessage + e.what());
	}
}

Script::~Script()
{
	// Do nothing
}

// Lua will search in this path when using require().
// Lua requires an absolute path!
bool Script::setLuaRequirePath(const std::string& absolutePath)
{
	try
	{
		LuaState luaState = mLuaContext.state();

		// From http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c/4156038#4156038
		luaState.getGlobal("package");
		luaState.getField(-1, "path");
		
		std::string currentPath = luaState.toString(-1);
		currentPath.append(";");
		currentPath.append(absolutePath + "?.lua"); // ?.lua seems to be needed and works better for cross-platform

		luaState.pop(1);
		luaState.push(currentPath.c_str());
		luaState.setField(-2, "path");
		luaState.pop(1);
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Failed to set Lua's require path for script '" + mName + "'! Error: ";
		Utils::CRASH(errorMessage + e.what());
		return false;
	}

	return true;
}

// All includes for the bindings
#include <Game.hpp>
#include <ResourceManager.hpp>
#include <InputManager.hpp>
#include <EntityManager.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <ObjectGeometryGroup.hpp>
#include <ObjectGeometry.hpp>
#include <Object.hpp>

#include <Definitions.hpp>
#include <glm/glm.hpp>
#include <SDL_keycode.h> // For key codes

// Binds all of the classes and functions. This creates our API!
// We need a Game instance to give it to the scripts.
void Script::bindInterface(Game& game)
{
	// Notes:
	// If you have a Lua error arg #-2 is nil, it is propably because the failing function is returning an object that is not binded!
	// You cannot give Lua different functions with the same name (overloaded Lua side). You have to give them different names (only on the Lua side).
	// Use a static_Cast to get the right overload.
	// You need to add a constructor to construct an object on the Lua side
	// You need to specify args with LUA_ARGS when binding a constructor with LUA_SP container (shared_ptr)

	LuaState luaState = mLuaContext.state();

	LuaBinding(luaState).addFunction("getGame", [&game]() -> Game& // The -> specifies the return type. The () are needed for this syntax.
	{
		return game;
	});

	LuaBinding(luaState).beginClass<Game>("Game")
		.addFunction("quit", &Game::quit) // &Game::quit returns quit()'s address
		.addFunction("setName", &Game::setName)
		.addFunction("setSize", &Game::setSize)
		.addFunction("setMainWindowPosition", &Game::setMainWindowPosition)
		.addFunction("reCenterMainWindow", &Game::reCenterMainWindow)

		.addFunction("getResourceManager", &Game::getResourceManager)
		.addFunction("getInputManager", &Game::getInputManager)
		.addFunction("getEntityManager", &Game::getEntityManager)
	.endClass();


	LuaBinding(luaState).beginClass<ResourceManager>("ResourceManager")
		.addConstant("BMP_TEXTURE", BMP_TEXTURE)
		.addConstant("DDS_TEXTURE", DDS_TEXTURE)

		.addFunction("addShader",
			// Specify which overload we want. Lua doesn't support functions with same names, though.
			static_cast<ResourceManager::shaderPointer(ResourceManager::*)(const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("addShaderWithName",
			static_cast<ResourceManager::shaderPointer(ResourceManager::*)(const std::string&, const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("findShader", &ResourceManager::findShader)

		.addFunction("addTexture",
			static_cast<ResourceManager::texturePointer(ResourceManager::*)(const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("addTextureWithName",
			static_cast<ResourceManager::texturePointer(ResourceManager::*)(const std::string&, const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("findTexture", &ResourceManager::findTexture)

		.addFunction("addObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*)(const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("addObjectGeometryGroupWithName",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*)(const std::string&, const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("findObjectGeometryGroup", &ResourceManager::findObjectGeometryGroup)
	.endClass();


	LuaBinding(luaState).beginModule("TextureType")
		.addConstant("BMP_TEXTURE", BMP_TEXTURE)
		.addConstant("DDS_TEXTURE", DDS_TEXTURE)
	.endModule();


	LuaBinding(luaState).beginClass<Shader>("Shader")
		.addFunction("getName", &Shader::getName)
	.endClass();


	LuaBinding(luaState).beginClass<Texture>("Texture")
		.addFunction("getName", &Texture::getName)
		.addFunction("getType", &Texture::getType)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometryGroup>("ObjectGeometryGroup")
		.addFunction("getName", &ObjectGeometryGroup::getName)
		.addFunction("addObjectGeometry", &ObjectGeometryGroup::addObjectGeometry)
		.addFunction("findObjectGeometry", &ObjectGeometryGroup::findObjectGeometry)
		.addFunction("getObjectGeometries", &ObjectGeometryGroup::findObjectGeometry)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry>("ObjectGeometry")
		// You can create this type of object, but it will be stored as an std::shared_ptr
		// You need to specify args with a shared pointer
		.addConstructor(LUA_SP(std::shared_ptr<ObjectGeometry>), LUA_ARGS(
			const std::string&,
			const ObjectGeometry::uintVector&,
			const ObjectGeometry::vec3Vector&,
			const ObjectGeometry::vec2Vector&,
			const ObjectGeometry::vec3Vector&))
		.addFunction("getName", &ObjectGeometry::getName)
	.endClass();


	LuaBinding(luaState).beginClass<InputManager>("InputManager")
		.addFunction("registerKey", &InputManager::registerKey)
		.addFunction("registerKeys", &InputManager::registerKeys)
		.addFunction("isKeyPressed", &InputManager::isKeyPressed)
	.endClass();


	// Bind SDL key codes (not all of them, we are lazy)
	LuaBinding(luaState).beginModule("KeyCode")
		.addConstant("SDLK_UP", SDLK_UP)
		.addConstant("SDLK_DOWN", SDLK_DOWN)
		.addConstant("SDLK_LEFT", SDLK_LEFT)
		.addConstant("SDLK_RIGHT", SDLK_RIGHT)
		.addConstant("SDLK_LSHIFT", SDLK_LSHIFT)
		.addConstant("SDLK_RSHIFT", SDLK_RSHIFT)
		.addConstant("SDLK_LCTRL", SDLK_LCTRL)
		.addConstant("SDLK_RCTRL", SDLK_RCTRL)
		.addConstant("SDLK_SPACE", SDLK_SPACE)
		.addConstant("SDLK_BACKSPACE", SDLK_BACKSPACE)
		.addConstant("SDLK_a", SDLK_a)
		.addConstant("SDLK_b", SDLK_b)
		.addConstant("SDLK_c", SDLK_c)
		.addConstant("SDLK_d", SDLK_d)
		.addConstant("SDLK_e", SDLK_e)
		.addConstant("SDLK_f", SDLK_f)
		.addConstant("SDLK_g", SDLK_g)
		.addConstant("SDLK_h", SDLK_h)
		.addConstant("SDLK_i", SDLK_i)
		.addConstant("SDLK_j", SDLK_j)
		.addConstant("SDLK_k", SDLK_k)
		.addConstant("SDLK_l", SDLK_l)
		.addConstant("SDLK_m", SDLK_m)
		.addConstant("SDLK_n", SDLK_n)
		.addConstant("SDLK_o", SDLK_o)
		.addConstant("SDLK_p", SDLK_p)
		.addConstant("SDLK_q", SDLK_q)
		.addConstant("SDLK_r", SDLK_r)
		.addConstant("SDLK_s", SDLK_s)
		.addConstant("SDLK_t", SDLK_t)
		.addConstant("SDLK_u", SDLK_u)
		.addConstant("SDLK_v", SDLK_v)
		.addConstant("SDLK_w", SDLK_w)
		.addConstant("SDLK_x", SDLK_x)
		.addConstant("SDLK_y", SDLK_y)
		.addConstant("SDLK_z", SDLK_z)
	.endModule();


	LuaBinding(luaState).beginClass<EntityManager>("EntityManager")
		.addFunction("getGameCamera", &EntityManager::getGameCamera)
		.addFunction("addObject", &EntityManager::addObject)
		.addFunction("getObjects", &EntityManager::getObjects)
		.addFunction("addLight", &EntityManager::addLight)
		.addFunction("getLights", &EntityManager::getLights)
	.endClass();


	LuaBinding(luaState).beginClass<Entity>("Entity")
		.addFunction("setPosition", &Entity::setPosition)
		.addFunction("getPosition", &Entity::getPosition)
		.addFunction("setScaling", &Entity::setScaling)
		.addFunction("getScaling", &Entity::getScaling)
		.addFunction("setRotation", &Entity::setRotation)
		.addFunction("getRotation", &Entity::getRotation)
		.addFunction("setVelocity", &Entity::setVelocity)
		.addFunction("getVelocity", &Entity::getVelocity)
	.endClass();


	// Entity must have at least one virtual function to be a base class
	//LuaBinding(luaState).beginExtendClass<Object, Entity>("Object")
	//.endClass();
}

// Running a script will probably not be too heavy since it will probably only be defining a bunch of callbacks.
// You should definitely call this after binding the interface.
void Script::run()
{
	try
	{
		mLuaContext.doString(mMainFileContents.c_str()); // Run the file!
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Script '" + mName + "' failed!\nError: ";
		Utils::CRASH(errorMessage + e.what());
	}
}