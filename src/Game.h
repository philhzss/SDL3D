#ifndef GAME_H_
#define GAME_H_

#include <GLAD/glad.h>
#include <SDL.h>

#include <ResourceManager.h>
#include <InputHandler.h>
#include <Camera.h>

#include <Object.h> // For testing
#include <TexturedObject.h> // For testing

class Game
{
private:
	std::string mGameName;
	std::string mLogFile;

	int mGameWidth;
	int mGameHeight;
	int mTicksPerFrame;

	SDL_Window *mMainWindow;
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager mResourceManager; // On stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp

	InputHandler mInputHandler; // A reference, when passed, does not copy the whole object: it only copies the reference.

	Camera mCamera;

	bool mQuitting; // If set to true, the game will quit at the end of the frame

	void checkCompability();
	void preMainLoopInit();
	void cleanUp();

	void doEvents();
	void render();
	void checkForErrors();
	void update();

	// Test
	TexturedObject *test;

public:
	Game(const std::string& gameName, int width, int height, int frameRate, const std::string& resourceDir);
	~Game();
	void init();
	void mainLoop();
	void quit();
};

#endif /* GAME_H_ */