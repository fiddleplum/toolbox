#pragma once

#include "gui_element.h"
#include "ptr.h"
#include <map>
#include <list>
#include <functional>

class GuiContainer : public GuiElement
{
public:
	Recti getBounds() const override;

	void setPosition(Coord2i position) override;

	void setSize(Coord2i size) override;

	template <typename T> Ptr<T> addElement();

	void removeElement(Ptr<GuiElement> const & element);

	void moveElementToFront(Ptr<GuiElement> const & element);

	void setElementActive(Ptr<GuiElement> const & element, bool active);

	void setElementPosition(Ptr<GuiElement> const & element, Coord2f fractionOfElement, Coord2f fractionOfContainer, Coord2i offset);

	void setElementSize(Ptr<GuiElement> const & element, Coord2f fractionOfContainer, Coord2i offset);

	void setContainerEventHandler(std::function<bool (Event const & event, Coord2i cursorPosition, bool cursorPositionIsValid)> handler);

	bool handleEvent(Event const & event, Coord2i cursorPosition, bool cursorPositionIsValid) override;

	void render(Coord2i windowSize) const override;

private:
	class ElementInfo
	{
	public:
		OwnPtr<GuiElement> element;
		bool active;
		Coord2f positionFractionOfElement;
		Coord2f positionFractionOfContainer;
		Coord2i positionOffset;
		Coord2f sizeFractionOfContainer;
		Coord2i sizeOffset;
	};

	std::list<ElementInfo>::iterator find(Ptr<GuiElement> const & element) const;
	void updateElementBounds(ElementInfo const & info);

	Recti bounds;
	std::list<ElementInfo> infos;
	std::map<Ptr<GuiElement>, std::list<ElementInfo>::iterator> lookup;
	std::function<bool (Event const & event, Coord2i cursorPosition, bool cursorPositionIsValid)> eventHandler;
};

template <typename T> Ptr<T> GuiContainer::addElement()
{
	ElementInfo info;
	info.element = OwnPtr<T>::createNew();
	info.active = true;
	info.sizeFraction = {1, 1};
	info.positionFractionOfElement = {0, 0};
	info.positionFractionOfContainer = {0, 0};
	info.positionOffset = {0, 0};
	auto it = elements.insert(elements.end(), info);
	lookup[info.element] = it;
	updateElementBounds(info);
	return info.element;
}

