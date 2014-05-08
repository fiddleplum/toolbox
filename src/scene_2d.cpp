#include "scene_2d.h"
#include "model_2d.h"
#include "camera_2d.h"
#include "controller.h"
#include "open_gl.h"
#include "frame_2d.h"

Scene2D::Scene2D()
{
}

void Scene2D::addCamera(std::shared_ptr<Camera2D> camera)
{
	cameras.insert(camera);
}

void Scene2D::removeCamera(std::shared_ptr<Camera2D> camera)
{
	cameras.erase(camera);
}

void Scene2D::addEntity(std::shared_ptr<Entity2D> entity)
{
	entities.insert(entity);
	entity->setSorted();
}

void Scene2D::removeEntity(std::shared_ptr<Entity2D> entity)
{
	entities.erase(entity);
}

void Scene2D::addController(std::shared_ptr<Controller> controller)
{
	controllers.insert(controller);
}

void Scene2D::removeController(std::shared_ptr<Controller> controller)
{
	controllers.erase(controller);
}

void Scene2D::handleEvent(Event const & event)
{
	for(auto controller : controllers)
	{
		controller->handleEvent(event);
	}
}

void Scene2D::render(std::shared_ptr<Camera2D> camera)
{
	// Set the OpenGL settings.
	glDisable(GL_DEPTH_TEST);

	// Check for sorting. Pull out the ones that need to be resorted, and put them back in the proper place.
	std::vector<std::shared_ptr<Entity2D>> entitiesToInsert;
	for(auto it = entities.begin(); it != entities.end();)
	{
		std::shared_ptr<Entity2D> entity = *it;
		if(!entity->isSorted())
		{
			it = entities.erase(it);
			entitiesToInsert.push_back(entity);
		}
		else
		{
			it++;
		}
	}
	for(auto entity : entitiesToInsert)
	{
		entities.insert(entity);
		entity->setSorted();
	}

	// Do the render.
	for(auto entity : entities)
	{
		entity->render(camera);
	}
}

