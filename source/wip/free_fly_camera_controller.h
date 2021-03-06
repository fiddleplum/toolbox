#pragma once

#include "controller.h"
#include <kit/coord.h>
#include <memory>

namespace kit
{
	class Camera;

	class FreeFlyCameraController : public Controller
	{
	public:
		FreeFlyCameraController();

		float getMoveSensitivity() const;
	
		void setMoveSensitivity(float);

		float getLookSensitivity() const;

		void setLookSensitivity(float);

		void setCamera(std::shared_ptr<Camera>);

		void handleEvent(Event const &) override;

	private:
		std::shared_ptr<Camera> camera;
		float lookSensitivity;
		float moveSensitivity;
		Coord3i moving;
		float yaw;
		float pitch;
		bool looking;
	};
}

