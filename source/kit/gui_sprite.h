#pragma once

#include "gui_element.h"
#include "ptr.h"
#include <string>

// TODO: Add ability to have a texture mask for determining the shape of the sprite when it comes to cursor clicks, etc.
// TODO: Add ability to have a base color (yellow, blue, transparent, etc) as vertex color that the texture lays on top of.
//   This would require modifying the gui shader.

class Texture;
class GuiModel;

class GuiSprite : public GuiElement
{
public:
	GuiSprite();

	Recti getBounds() const override;

	void setPosition(Coord2i position) override;

	void setSize(Coord2i size) override;

	Recti getTextureBounds() const;

	void setTextureBounds(Recti bounds);

	void setTexture(Ptr<Texture> texture);

	void setTexture(std::string const & filename);

	Ptr<Texture> getTexture() const;

	bool handleEvent(Event const & event, Coord2i cursorPosition, bool cursorPositionIsValid) override;

	void render(Coord2i windowSize) const override;

private:
	void updateVertices();

	OwnPtr<GuiModel> model;
	Coord2i position;
	Recti textureBounds;
};

