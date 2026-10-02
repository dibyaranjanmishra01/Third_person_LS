#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <string>
#include <filesystem>
#include <array>
#include <span>
#include <iostream>

#include "graphics/ShaderLoader.h"
#include "graphics/Vertex.h"
#include "app/AppState.h"
#include "input/InputState.h"

// This function runs once at startup
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_Log("Init");
	// Continue running the application

    appstate::AppState* myAppState = new appstate::AppState();
    if(!myAppState->Initialize())
    {
        delete myAppState;
        return SDL_APP_FAILURE;
    }
    *appstate = myAppState;

	return SDL_APP_CONTINUE;
}

// This function runs when a new event (mouse input, keypresses, etc) occurs
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	// SDL_Log("Event");
	appstate::AppState* myAppState = static_cast<appstate::AppState*>(appstate);

	switch (event->type)
	{
	case SDL_EVENT_MOUSE_MOTION:
        // inputState.mouseDeltaX += event.motion.xrel;
        // inputState.mouseDeltaY += event.motion.yrel;
		myAppState->addMouseInput(event->motion.xrel, event->motion.yrel);
	break;
	case SDL_EVENT_QUIT:
		// Quit the application with a success state
		return SDL_APP_SUCCESS;
	default:
		// Continue running the application
		return SDL_APP_CONTINUE;
	}
}

// This function runs once per frame, and is the heart of the application
SDL_AppResult SDL_AppIterate(void* appstate)
{
	// SDL_Log("Iterate");
	// Continue running the application
    appstate::AppState* myAppState = static_cast<appstate::AppState*>(appstate);
    
    if(!myAppState->loop())
    {
        return SDL_APP_FAILURE;
    }

	return SDL_APP_CONTINUE;
}

// This function runs once at shutdown
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    appstate::AppState* myAppState = static_cast<appstate::AppState*>(appstate);
    delete myAppState;
	SDL_Log("Quit");
}
