#pragma once

#include "Widget.h"
#include "Model.h"
#include <memory>

namespace Gui
{
	class Sprite : public Widget
	{
	public:
		Sprite();
		virtual ~Sprite();

		Box2i getBounds() const override;
		void setPosition(Vector2i position) override;
		void setMaxSize(Vector2i maxSize) override;
		void handleEvent(Event const & event) override;
		void render() override;

		void setTexture(std::string const & filename);
		void setUVBounds(Box2i bounds);

	private:
		void updateVertices();

		Model model;
		Box2i uvBounds;
		Vector2i maxSize;
	};
}

