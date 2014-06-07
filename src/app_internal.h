#pragma once

#include "window_internal.h"
#include <kit/app.h>
#include "resource_manager.h"

namespace kit
{
	class Texture;
	class Shader;
	class AppInternal;

	Ptr<AppInternal> app ();

	class AppInternal : public App
	{
	public:
		AppInternal ();

		~AppInternal ();

		void loop ();

		void quit () override;

		Ptr<Window> addWindow (char const * title) override;

		void removeWindow (Ptr<Window> window) override;

		void setCursorValid (bool valid);

		Ptr<ResourceManager<Texture>> getTextureManager();

		Ptr<ResourceManager<Shader>> getShaderManager();

	private:
		Ptr<WindowInternal> getWindowFromId (Uint32 id);
		KeyboardEvent::Key sdlKeyToKey (int sdlKey);
		void handleSDLEvent (SDL_Event const & sdlEvent);

		std::map<Ptr<Window>, OwnPtr<WindowInternal>> windows;
		SDL_GLContext sdlGlContext;
		bool looping;
		bool cursorIsValid;
		bool firstMouseMoveEvent;
		std::vector<SDL_Joystick *> joysticks;
		OwnPtr<ResourceManager<Texture>> textureManager;
		OwnPtr<ResourceManager<Shader>> shaderManager;
	};
}

