#include "model.h"
#include "camera.h"
#include "frame.h"
#include "../shader.h"
#include "../texture.h"
#include "../vertex_buffer_object.h"
#include "../app.h"
#include "../serialize.h"
#include "../serialize_std_string.h"
#include "../serialize_std_vector.h"
#include <kit/string_util.h>
#include <fstream>
#include <stdexcept>

namespace kit
{
	namespace scene
	{
		Model::Model ()
		{
			p.set(new P);
			p->vertexHasNormal = false;
			p->vertexHasTangent = false;
			p->vertexHasColor = false;
			p->numVertexUVs = 0;
			p->emitColor = Vector3f(0, 0, 0);
			p->diffuseColor = Vector4f(1, 1, 1, 1);
			p->specularLevel = 1;
			p->specularStrength = 0;
			p->scale = 1;
			p->vertexBufferObject.set(new VertexBufferObject);
			p->vertexBufferObject->setBytesPerVertex(sizeof(Vector3f));
			p->updateShader();
			p->needsResorting = true;
		}

		Model::Model (std::string const & filename)
		{
			std::fstream in (filename, std::fstream::in | std::fstream::binary);

			// Material
			Vector3f emitColor;
			deserialize(in, emitColor[0]);
			deserialize(in, emitColor[1]);
			deserialize(in, emitColor[2]);
			Vector4f diffuseColor;
			deserialize(in, diffuseColor[0]);
			deserialize(in, diffuseColor[1]);
			deserialize(in, diffuseColor[2]);
			deserialize(in, diffuseColor[3]);
			setColor(emitColor, diffuseColor);
			unsigned int specularLevel;
			float specularStrength;
			deserialize(in, specularLevel);
			deserialize(in, specularStrength);
			setSpecular(specularLevel, specularStrength);

			// Textures
			clearTextures();
			unsigned int numTextures;
			deserialize(in, numTextures);
			for(unsigned int i = 0; i < numTextures; i++)
			{
				std::string filename;
				deserialize(in, filename);
				std::string type;
				deserialize(in, type);
				unsigned int uvIndex;
				deserialize(in, uvIndex);
				addTexture(filename, type, uvIndex);
			}

			// Vertex format
			bool vertexHasNormal, vertexHasTangent, vertexHasColor;
			unsigned int numVertexUVs;
			deserialize(in, vertexHasNormal);
			deserialize(in, vertexHasTangent);
			deserialize(in, vertexHasColor);
			deserialize(in, numVertexUVs);
			setVertexFormat(vertexHasNormal, vertexHasTangent, vertexHasColor, numVertexUVs);

			// Vertices
			unsigned int numVertices;
			deserialize(in, numVertices);
			std::vector<unsigned char> vertices;
			vertices.resize(numVertices * p->numBytesPerVertex);
			deserialize(in, (void *)&vertices[0], numVertices * p->numBytesPerVertex);
			setVertices((void const *)&vertices[0], numVertices * p->numBytesPerVertex);

			// Index format
			unsigned int numIndicesPerPrimitive;
			deserialize(in, numIndicesPerPrimitive);
			setNumIndicesPerPrimitive(numIndicesPerPrimitive);

			// Indices
			std::vector<unsigned int> indices;
			deserialize<unsigned int>(in, indices, deserialize);
			setIndices(&indices[0], indices.size());
		}

		void Model::setVertexFormat (bool hasNormal, bool hasTangent, bool hasColor, unsigned int numVertexUVs)
		{
			p->numBytesPerVertex = sizeof(Vector3f); // position
			p->vertexHasNormal = hasNormal;
			if(p->vertexHasNormal)
			{
				p->numBytesPerVertex += sizeof(Vector3f);
			}
			p->vertexHasTangent = hasTangent;
			if(p->vertexHasTangent)
			{
				p->numBytesPerVertex += sizeof(Vector3f);
			}
			p->vertexHasColor = hasColor;
			if(p->vertexHasColor)
			{
				p->numBytesPerVertex += sizeof(Vector4f);
			}
			p->numVertexUVs = numVertexUVs;
			p->numBytesPerVertex += numVertexUVs * 2 * sizeof(Vector2f);
			p->vertexBufferObject->setBytesPerVertex(p->numBytesPerVertex);
			p->updateShader();
		}

		void Model::setVertices (void const * vertices, unsigned int numBytes)
		{
			p->vertexBufferObject->setVertices(vertices, numBytes, false);
		}

		void Model::setNumIndicesPerPrimitive (unsigned int num)
		{
			p->vertexBufferObject->setNumIndicesPerPrimitive(num);
		}

		void Model::setIndices (unsigned int const * indices, unsigned int numIndices)
		{
			p->vertexBufferObject->setIndices(indices, numIndices);
		}

		Ptr<kit::Texture> Model::getTexture (unsigned int textureIndex) const
		{
			if(textureIndex < p->textureInfos.size())
			{
				return p->textureInfos[textureIndex].texture;
			}
			else
			{
				Ptr<Texture>();
			}
		}

		void Model::addTexture (std::string const & filename, std::string const & type, unsigned int uvIndex)
		{
			P::TextureInfo textureInfo;
			textureInfo.texture = app()->getResources()->getTextureFromFile(filename).as<Texture>();
			textureInfo.type = type;
			textureInfo.samplerLocation = -1;
			textureInfo.uvIndex = uvIndex;
			p->textureInfos.push_back(textureInfo);
			p->updateShader();
		}

		void Model::clearTextures ()
		{
			textureInfos.clear();
			updateShader();
		}

		void Model::setColor (Vector3f const & emitColor, Vector4f const & diffuseColor)
		{
			this->emitColor = emitColor;
			this->diffuseColor = diffuseColor;
		}

		void Model::setSpecular (unsigned int level, float strength)
		{
			specularLevel = level;
			specularStrength = strength;
		}

		void Model::setScale (float scale)
		{
			this->scale = scale;
		}		

		void Model::render (Ptr<Camera> camera, Frame const & frame, std::vector<Vector3f> const & lightPositions, std::vector<Vector3f> const & lightColors) const
		{
			// The render engine handles shader and texture activation.
			shader->activate();
			shader->setUniform(projectionLocation, camera->getProjection());
			shader->setUniform(worldViewLocation, camera->getView() * frame.getMatrix());
			shader->setUniform(scaleLocation, scale);
			unsigned int samplerIndex = 0;
			for(unsigned int i = 0; i < textureInfos.size(); i++)
			{
				textureInfos[i].texture->activate(i);
				shader->setUniform(textureInfos[i].samplerLocation, (int)samplerIndex);
				samplerIndex++;
			}
			Texture::deactivateRest(textureInfos.size());
			if(lightPositions.size() > 0)
			{
				shader->setUniform(lightPositionsLocation, &lightPositions[0], lightPositions.size());
				shader->setUniform(lightColorsLocation, &lightColors[0], lightColors.size());
			}
			shader->setUniform(emitColorLocation, emitColor);
			shader->setUniform(diffuseColorLocation, diffuseColor);
			shader->setUniform(specularLevelLocation, (int)specularLevel);
			shader->setUniform(specularStrengthLocation, specularStrength);
			vertexBufferObject->render();
		}

		bool Model::setNeedsResorting () const
		{
			return needsResorting;
		}

		void Model::resortingDone ()
		{
			needsResorting = false;
		}

		bool Model::operator < (Model const & model) const
		{
			if(shader < model.shader)
			{
				return true;
			}
			else if(shader > model.shader)
			{
				return false;
			}
			for(unsigned int i = 0; i < std::min(textureInfos.size(), model.textureInfos.size()); i++)
			{
				if(textureInfos[i].texture < model.textureInfos[i].texture)
				{
					return true;
				}
				else if(textureInfos[i].texture > model.textureInfos[i].texture)
				{
					return false;
				}
			}
			if(textureInfos.size() < model.textureInfos.size())
			{
				return true;
			}
			else if(textureInfos.size() > model.textureInfos.size())
			{
				return false;
			}
			return vertexBufferObject < model.vertexBufferObject;
		}

		void Model::updateShader ()
		{
			std::string code [Shader::NumCodeTypes];

			std::vector<std::string> uvIndexStrings;
			for(unsigned int uvIndex = 0; uvIndex < numVertexUVs; uvIndex++)
			{
				uvIndexStrings.push_back(std::to_string(uvIndex));
			}

			/** VERTEX **/
			code[Shader::Vertex] += "#version 150\n";

			// Add the global variables.
			code[Shader::Vertex] += "uniform mat4 uWorldView;\n";
			code[Shader::Vertex] += "uniform mat4 uProjection;\n";
			code[Shader::Vertex] += "uniform float scale;\n";
			code[Shader::Vertex] += "in vec3 aPosition;\n";
			code[Shader::Vertex] += "out vec3 vPosition;\n";
			if(vertexHasNormal)
			{
				code[Shader::Vertex] += "in vec3 aNormal;\n";
				code[Shader::Vertex] += "out vec3 vNormal;\n";
			}
			if(vertexHasTangent)
			{
				code[Shader::Vertex] += "in vec3 aTangent;\n";
				code[Shader::Vertex] += "out vec3 vTangent;\n";
			}
			if(vertexHasColor)
			{
				code[Shader::Vertex] += "in vec4 aColor;\n";
				code[Shader::Vertex] += "out vec4 vColor;\n";
			}
			for(unsigned int uvIndex = 0; uvIndex < numVertexUVs; uvIndex++)
			{
				code[Shader::Vertex] += "in vec2 aUV" + uvIndexStrings[uvIndex] + ";\n";
				code[Shader::Vertex] += "out vec2 vUV" + uvIndexStrings[uvIndex] + ";\n";
			}

			// Add the main function.
			code[Shader::Vertex] += "void main()\n";
			code[Shader::Vertex] += "{\n";
			code[Shader::Vertex] += "	gl_Position = uProjection * uWorldView * vec4(scale * aPosition, 1);\n";
			code[Shader::Vertex] += "	vPosition = (uWorldView * vec4(aPosition, 1)).xyz;\n";
			if(vertexHasNormal)
			{
				code[Shader::Vertex] += "	vNormal = (uWorldView * vec4(aNormal, 0)).xyz;\n";
			}
			if(vertexHasTangent)
			{
				code[Shader::Vertex] += "	vTangent = (uWorldView * vec4(aTangent, 0)).xyz;\n";
			}
			if(vertexHasColor)
			{
				code[Shader::Vertex] += "	vColor = aColor;\n";
			}
			for(unsigned int uvIndex = 0; uvIndex < numVertexUVs; uvIndex++)
			{
				code[Shader::Vertex] += "	vUV" + uvIndexStrings[uvIndex] + " = aUV" + uvIndexStrings[uvIndex] + ";\n";
			}
			code[Shader::Vertex] += "}\n";

			/** FRAGMENT **/
			code[Shader::Fragment] += "#version 150\n";

			// Add the global variables.
			code[Shader::Fragment] += "in vec3 vPosition;\n";
			code[Shader::Fragment] += "uniform vec3 uEmitColor;\n";
			if(vertexHasNormal)
			{
				code[Shader::Fragment] += "uniform vec3 uLightPositions [" + std::to_string(maxLights) + "];\n";
				code[Shader::Fragment] += "uniform vec3 uLightColors [" + std::to_string(maxLights) + "];\n";
				code[Shader::Fragment] += "in vec3 vNormal;\n";
				if(vertexHasTangent)
				{
					code[Shader::Fragment] += "in vec3 vTangent;\n";
				}
			}
			if(vertexHasColor)
			{
				code[Shader::Fragment] += "in vec4 vColor;\n";
			}
			else
			{
				code[Shader::Fragment] += "uniform vec4 uDiffuseColor;\n";
			}
			for(unsigned int uvIndex = 0; uvIndex < numVertexUVs; uvIndex++)
			{
				code[Shader::Fragment] += "in vec2 vUV" + uvIndexStrings[uvIndex] + ";\n";
			}
			for(unsigned int samplerIndex = 0; samplerIndex < textureInfos.size(); samplerIndex++)
			{
				code[Shader::Fragment] += "uniform sampler2D uSampler" + std::to_string(samplerIndex) + ";\n";
			}

			// Add the main function.
			code[Shader::Fragment] += "void main()\n";
			code[Shader::Fragment] += "{\n";
			if(vertexHasColor)
			{
				code[Shader::Fragment] += "	vec4 color = vColor;\n";
			}
			else
			{
				code[Shader::Fragment] += "	vec4 color = uDiffuseColor;\n";
			}
			unsigned int samplerIndex = 0;
			for(TextureInfo const & textureInfo : textureInfos)
			{
				std::string samplerIndexString = std::to_string(samplerIndex);
				if(textureInfo.type == "diffuse")
				{
					code[Shader::Fragment] += "	vec4 textureColor" + samplerIndexString + " = texture2D(uSampler" + samplerIndexString + ", vUV" + std::to_string(textureInfo.uvIndex) + ");\n";
					code[Shader::Fragment] += "	color = (1.0f - textureColor" + samplerIndexString + ".w) * color + textureColor" + samplerIndexString + ".w * textureColor" + samplerIndexString + ";\n";
				}
				else if(textureInfo.type == "normal")
				{
				}
				else if(textureInfo.type == "reflection")
				{
				}
				samplerIndex++;
			}
			if(vertexHasNormal)
			{
				code[Shader::Fragment] += "	gl_FragColor = vec4(0, 0, 0, color.a);\n";
				code[Shader::Fragment] += "	for(int i = 0; i < " + std::to_string(maxLights) + "; i++)\n";
				code[Shader::Fragment] += "	{\n";
				code[Shader::Fragment] += "		float dotLight = dot(normalize(uLightPositions[i] - vPosition), vNormal);\n";
				code[Shader::Fragment] += "		if(dotLight > 0)\n";
				code[Shader::Fragment] += "		{\n";
				code[Shader::Fragment] += "			gl_FragColor.rgb += color.rgb * uLightColors[i] * dotLight;\n";
				code[Shader::Fragment] += "		}\n";
				code[Shader::Fragment] += "	}\n";
			}
			else
			{
				code[Shader::Fragment] += "	gl_FragColor = color;\n";
			}
			code[Shader::Fragment] += "	gl_FragColor.rgb += uEmitColor;\n";
			code[Shader::Fragment] += "}\n";

			// Create a unique name for the shader.
			std::string name;
			name = "Model";
			if(vertexHasNormal)
			{
				name += "n";
			}
			if(vertexHasTangent)
			{
				name += "t";
			}
			if(vertexHasColor)
			{
				name += "c";
			}
			name += std::to_string(numVertexUVs);
			for(TextureInfo const & textureInfo : textureInfos)
			{
				name += textureInfo.type[0] + std::to_string(textureInfo.uvIndex);
			}

			shader = app()->getResources()->getShader(name, code);
			needsResorting = true;

			// Update attribute locations
			vertexBufferObject->clearVertexComponents();
			unsigned int offset = 0;
			vertexBufferObject->addVertexComponent(shader->getAttributeLocation("aPosition"), offset, 3);
			offset += sizeof(Vector3f);
			if(vertexHasNormal)
			{
				vertexBufferObject->addVertexComponent(shader->getAttributeLocation("aNormal"), offset, 3);
				offset += sizeof(Vector3f);
			}
			if(vertexHasTangent)
			{
				vertexBufferObject->addVertexComponent(shader->getAttributeLocation("aTangent"), offset, 3);
				offset += sizeof(Vector3f);
			}
			if(vertexHasColor)
			{
				vertexBufferObject->addVertexComponent(shader->getAttributeLocation("aColor"), offset, 4);
				offset += sizeof(Vector4f);
			}
			for(TextureInfo const & textureInfo : textureInfos)
			{
				vertexBufferObject->addVertexComponent(shader->getAttributeLocation("aUV" + std::to_string(textureInfo.uvIndex)), offset + textureInfo.uvIndex * sizeof(Vector2f), 2);
			}

			// Update uniform locations
			lightPositionsLocation = shader->getUniformLocation("uLightPositions");
			lightColorsLocation = shader->getUniformLocation("uLightColors");
			emitColorLocation = shader->getUniformLocation("uEmitColor");
			diffuseColorLocation = shader->getUniformLocation("uDiffuseColor");
			specularLevelLocation = shader->getUniformLocation("uSpecularLevel");
			specularStrengthLocation = shader->getUniformLocation("uSpecularStrength");
			scaleLocation = shader->getUniformLocation("uScale");
			for(TextureInfo & textureInfo : textureInfos)
			{
				textureInfo.samplerLocation = shader->getUniformLocation("uSampler" + std::to_string(samplerIndex));
			}
			projectionLocation = shader->getUniformLocation("uProjection");
			worldViewLocation = shader->getUniformLocation("uWorldView");
		}
	}
}

/*

Model File Format

strings are UTF-8 encoded. They have the number of bytes prepended as an unsigned int. They are not null-terminated.
bools is 8 bits.
ints, unsigned ints, and floats are 32 bits.
lists begin with an unsigned int indicating their length and then the elements tightly packed.

model:
material - the material
bool - has normal
bool - has tangent
bool - has color
unsigned int - num uvs
vertex list - list of vertex components (vertices all contatenated, tightly packed)
unsigned int - number of indices per primitive
int list - list of indices that make up triangles

material:
float[3] - emmisive color (0..1)
float[4] - diffuse color (0..1)
unsigned int - shininess (>= 1)
float - shininess strength (>= 0)
texture list - list of textures

texture:
string - filename
string - type
int - uv index

vertex format:
float[3] - position
float[3] - normal (if it has one)
float[3] - tangent (if it has one)
float[4] - color (if it has one)
float[2] list - uvs *** Note: this list is not prepended with a length.

*/

