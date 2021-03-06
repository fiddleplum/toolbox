#pragma once

#include <set>
#include <memory>

class Event;

namespace Scene2D
{
	class Camera;
	class Model;
	class Controller;

	class Scene
	{
	public:
		Scene();

		void addCamera(std::shared_ptr<Camera> camera);

		void removeCamera(std::shared_ptr<Camera> camera);

		void addModel(std::shared_ptr<Model> model);

		void removeModel(std::shared_ptr<Model> model);

		void addController(std::shared_ptr<Controller> controller);

		void removeController(std::shared_ptr<Controller> controller);

		// Called my Camera.
		void handleEvent(Event const & event);

		// Called by Camera.
		void render(std::shared_ptr<Camera> camera);

	private:
		Scene(Scene const &);
		Scene & operator =(Scene const &);

		class ModelCompare
		{
		public:
			bool operator ()(std::shared_ptr<Model> model0, std::shared_ptr<Model> model1);
		};

		std::set<std::shared_ptr<Camera>> cameras;
		std::set<std::shared_ptr<Model>, ModelCompare> mModels;
		std::set<std::shared_ptr<Controller>> controllers;
	};
}

