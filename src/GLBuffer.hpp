//// Copyright 2015 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// A simple general auto-binding OpenGL buffer wrapper
// Useful for using as an interface for other classes (retun this instead of writing an interface)

#ifndef GLBUFFER_HPP_
#define GLBUFFER_HPP_

#include <GLAD/glad.h>
#include <vector>

#include <cstdlib> // For size_t

template<typename bufferDataType>
class GLBuffer
{
private:
	GLuint mID; // OpenGL handle
	bool mAutoBind;
	GLenum mTarget; // The target to bind to

public:
	// Even if auto binding is not on, calling bind() will still bind to the default target
	GLBuffer(bool autoBind = true, GLenum target = GL_ARRAY_BUFFER)
	{
		mAutoBind = autoBind;
		setTarget(target);

		glGenBuffers(1, &mID); // 1 for 1 buffer
	}

	~GLBuffer()
	{
		glDeleteBuffers(1, &mID);
	}

	// Copy constructor, makes a new OpenGL buffer. Unbinds copy buffers!
	GLBuffer(const GLBuffer& other)
	{
		// For newer OpenGL versions: int oldCopyWriteBuffer = glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING);
		// int oldCopyReadBuffer = glGetIntegerv(GL_COPY_READ_BUFFER_BINDING);

		mAutoBind = other.mAutoBind;
		setTarget(other.mTarget);
		
		glGenBuffers(1, &mID);

		// Code to make sure the data and the flags are the same
		GLint isImmutable;
		other.bind();
		glGetBufferParameteriv(other.mTarget, GL_BUFFER_IMMUTABLE_STORAGE, &isImmutable);
		
		if(isImmutable)
		{
			GLint immutableFlags;
			other.bind();
			glGetBufferParameteriv(other.mTarget, GL_BUFFER_STORAGE_FLAGS, &immutableFlags);
			
			setImmutableData(other.readData(), immutableFlags);
		} else
		{
			GLint usage;
			other.bind();
			glGetBufferParameteriv(other.mTarget, GL_BUFFER_USAGE, &usage);

			setMutableData(other.readData(), usage);
		}
	}

	const GLuint getID()
	{
		return mID;
	}

	void setTarget(GLenum target)
	{
		mTarget = target;
	}

	void bind(GLenum target) const
	{
		if(mAutoBind)
			glBindBuffer(target, mID);
	}

	void bind() const // Bind to the stored target
	{
		bind(mTarget);
	}

	size_t getSize() const // Returns the buffer's size, in bytes. size_t is not in the std namespace since it is C, not C++.
	{
		GLint GLintBufferSize;

		bind();
		glGetBufferParameteriv(mTarget, GL_BUFFER_SIZE, &GLintBufferSize);

		return GLintBufferSize; // Implicit conversion to size_t
	}

	int getLength() const // Get the amount of elements in the buffer
	{
		int bufferSize = getSize();

		return bufferSize / sizeof(bufferDataType);
	}

	void setMutableData(const std::vector<bufferDataType>& data, GLenum usage)
	{
		bind();
		glBufferData(mTarget, sizeof(bufferDataType) * data.size(), &data[0], usage); // vector.size() returns the amount of elements
	}

	void setImmutableData(const std::vector<bufferDataType>& data, GLenum immutableFlags) // immutableFlags being a bitwise operation
	{
		bind();
		glBufferStorage(mTarget, sizeof(bufferDataType) * data.size(), &data[0], immutableFlags);
	}

	std::vector<bufferDataType> readData(GLintptr offset, GLsizeiptr size) const
	{
		std::vector<bufferDataType> data(size / sizeof(bufferDataType)); // Allocate
		glGetBufferSubData(mTarget, offset, size, &data[0]);

		return data;
	}

	// Read all of the data
	std::vector<bufferDataType> readData() const
	{
		return readData(0, getSize());
	}

	// If the buffer is immutable, make sure you gave the right immutableFlags to make it changeable
	void modifyData(GLintptr offset, const std::vector<bufferDataType>& data)
	{
		bind();
		glBufferSubData(mTarget, offset, Utils::getSizeOfVectorData(vertices), &data[0]);
	}
};

#endif /* GLBUFFER_HPP_ */