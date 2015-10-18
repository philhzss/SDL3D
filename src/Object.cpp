// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#include <Object.hpp>
#include <HelperFunctions.hpp> // For vector stuff and error messages

#include <sstream>
#include <fstream> // For file stuff

// Uniforms:
// - mat4 MVP

// In:
// - layout location 0: vertex position in modelspace

using namespace HelperFunctions;

Object::Object(constShaderPointer shaderPointer) // You can set the vertex buffer manually!
{
	mShaderPointer = shaderPointer;
}

Object::Object(vec3Vector& vertices, constShaderPointer shaderPointer)
{
	mShaderPointer = shaderPointer;

	mVertexBuffer.setMutableData(vertices, GL_DYNAMIC_DRAW); // DYNAMIC_DRAW as a hint to OpenGL that we might change the vertices
}

Object::Object(const std::string& objectPath, constShaderPointer shaderPointer) // Uses std::array for modernism, wasn't necessairy
{
	mShaderPointer = shaderPointer;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> normals;
	loadOBJData(objectPath, vertices, UVs, normals);

	// Only keep vertices in this case
	mVertexBuffer.setMutableData(vertices, GL_DYNAMIC_DRAW); // DYNAMIC_DRAW as a hint to OpenGL that we might change the vertices
}

Object::~Object()
{
	// Do nothing
}

// Static
// A super-simple .obj loader
bool Object::loadOBJData(const std::string& filePath,
						 std::vector<glm::vec3>& outVertices,
						 std::vector<glm::vec2>& outUVs,
						 std::vector<glm::vec3>& outNormals)
{
	std::vector<unsigned int> vertexIndices, UVIndices, normalIndices; // Indices only, so ints

	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;

	std::ifstream file(filePath);
	std::string line;
	
	if(!file)
	{
		crash("The object " + filePath + " does not exist or cannot be opened!");
		return false;
	}
	
	while(std::getline(file, line)) // Give the line string so that getLine() can fill it
	{
		std::vector<std::string> words = splitString(line, ' ');

		if(words[0] == "v")
		{
			glm::vec3 vertex(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
			tempVertices.push_back(vertex);
		} else if(words[0] == "vt")
		{
			glm::vec2 UV(std::stof(words[1]), std::stof(words[2]));
			tempUVs.push_back(UV);
		} else if(words[0] == "vn")
		{
			glm::vec3 normal(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
			tempNormals.push_back(normal);
		} else if(words[0] == "f")
		{
			std::vector<std::string> vertex0 = splitString(words[1], '/');
			std::vector<std::string> vertex1 = splitString(words[2], '/');
			std::vector<std::string> vertex2 = splitString(words[3], '/');
			
			if(vertex0.size() != 3 || vertex1.size() != 3 || vertex2.size() != 3)
			{
				crash("Object at " + filePath + " is badly formatted for our simple loader!");
				return false;
			}
			
			vertexIndices.push_back(std::stoi(vertex0[0]));
			UVIndices.push_back(std::stoi(vertex0[1]));
			normalIndices.push_back(std::stoi(vertex0[2]));

			vertexIndices.push_back(std::stoi(vertex1[0]));
			UVIndices.push_back(std::stoi(vertex1[1]));
			normalIndices.push_back(std::stoi(vertex1[2]));

			vertexIndices.push_back(std::stoi(vertex2[0]));
			UVIndices.push_back(std::stoi(vertex2[1]));
			normalIndices.push_back(std::stoi(vertex2[2]));
		}
	}
	
	for(unsigned int i=0; i<vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int UVIndex = UVIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// The actual vertex for this one
		glm::vec3 vertex = tempVertices[vertexIndex-1]; // -1 since .obj indexes are 1-based
		outVertices.push_back(vertex);

		glm::vec2 UV = tempUVs[UVIndex-1];
		outUVs.push_back(UV);

		glm::vec3 normal = tempNormals[normalIndex-1];
		outNormals.push_back(normal);
	}

	return true;
}

Object::vec3Buffer& Object::getVertexBuffer()
{
	return mVertexBuffer;
}

void Object::setShader(constShaderPointer shaderPointer)
{
	mShaderPointer = shaderPointer;
}

Object::constShaderPointer Object::getShader()
{
	return mShaderPointer;
}

// Virtual
void Object::render(glm::mat4 MVP)
{
	glUseProgram(mShaderPointer->getID());
	glUniformMatrix4fv(mShaderPointer->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0); // Number to give to OpenGL VertexAttribPointer
	mVertexBuffer.bind(GL_ARRAY_BUFFER);

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, mVertexBuffer.getLength()); // Draw!
	glDisableVertexAttribArray(0);
}
