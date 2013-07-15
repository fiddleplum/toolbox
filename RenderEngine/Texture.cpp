#include "Texture.h"
#include "OpenGL.h"
#include <SDL_Image.h>
#include <stdexcept>
#include <fstream>

Texture::Texture(std::string const & filename)
{
	mFilename = filename;
	glGenTextures(1, &mHandle);
}

Texture::~Texture()
{
	glDeleteTextures(1, &mHandle);
}

std::string Texture::toString() const
{
	return mFilename;
}

void Texture::activate(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mHandle);
}

void Texture::deactivate(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

