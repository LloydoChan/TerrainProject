//Sea.h 
#pragma once

#include "Includes.h"
#include "Quad.h"
#include "Shader.h"

using glm::mat4x4;

const GLfloat seaQuadData[] = {
	-1.0f,  0.0f, 1.0f,

	1.0f,  0.0f, 1.0f,
	-1.0f, 0.0f, -1.0f,
	1.0f, 0.0f, -1.0f
};

class Sea{
	public:
		Sea(){};
		~Sea(){};

		bool Init();
		void Draw(const mat4x4& MVPMatrix);

	private:
		Shader* m_shader;
		Quad*   m_quad;
		mat4x4  m_scale;
};