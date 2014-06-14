#include "sprite_p.h"
#include "model.h"

namespace kit
{
	namespace gui
	{
		OwnPtr<Sprite> Sprite::create ()
		{
			return OwnPtr<SpriteP> (new SpriteP);
		}

		SpriteP::SpriteP ()
		{
			model.set(new Model);
			std::vector<unsigned int> indices (6);
			indices[0] = 0;
			indices[1] = 3;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 1;
			indices[5] = 0;
			model->setIndices(indices);
			updateVertices();
		}

		Recti SpriteP::getBounds () const
		{
			return Recti::minSize(model->getPosition(), textureBounds.getSize());
		}

		void SpriteP::setPosition (Vector2i position)
		{
			model->setPosition(position);
		}

		void SpriteP::setMaxSize (Vector2i maxSize)
		{
			this->maxSize = maxSize;
			updateVertices();
		}

		Ptr<Texture> SpriteP::getTexture () const
		{
			// TODO
			return Ptr<Texture> ();
		}

		void SpriteP::setNewTexture (Vector2i)
		{
			// TODO
		}

		void SpriteP::setTexture (std::string const & filename)
		{
			model->setTexture(filename);
		}

		Recti SpriteP::getTextureBounds () const
		{
			return textureBounds;
		}

		void SpriteP::setTextureBounds (Recti bounds)
		{
			textureBounds = bounds;
			updateVertices();
		}

		bool SpriteP::handleEvent (Event const &, bool cursorIsValid)
		{
			return cursorIsValid;
		}

		void SpriteP::render (Vector2i windowSize)
		{
			model->render(windowSize);
		}

		void SpriteP::updateVertices()
		{
			Vector2i size;
			size[0] = std::min(textureBounds.getSize()[0], maxSize[0]);
			size[1] = std::min(textureBounds.getSize()[1], maxSize[1]);
			std::vector<Model::Vertex> vertices (4);
			vertices[0].pos.set(0, 0);
			vertices[0].uv.set((float)textureBounds.min[0], (float)textureBounds.min[1]);
			vertices[1].pos.set((float)size[0], 0);
			vertices[1].uv.set((float)textureBounds.min[0] + (float)size[0], (float)textureBounds.min[1]);
			vertices[2].pos.set((float)size[0], (float)size[1]);
			vertices[2].uv.set((float)textureBounds.min[0] + (float)size[0], (float)textureBounds.min[1] + (float)size[1]);
			vertices[3].pos.set(0, (float)size[1]);
			vertices[3].uv.set((float)textureBounds.min[0], (float)textureBounds.min[1] + (float)size[1]);
			model->setVertices(vertices);
		}
	}
}

