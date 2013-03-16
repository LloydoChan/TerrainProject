//Sea.cpp
#include "Sea.h"

bool Sea::Init(){
  m_quad = new Quad();
  m_quad->Init((float*)seaQuadData);

  m_shader = new Shader();
  m_shader->InitShaderProgramFromFile("shaders\\seaShader.vp","shaders\\seaShader.fp",1,"inCoords");
  m_scale = glm::scale(2000.0f,1.0f,2000.0f);
  return true;
}

void Sea::Draw(const mat4x4& MVPMatrix){
	glUseProgram(m_shader->GetHandle());
		m_shader->SetUniform("MVPMatrix",MVPMatrix * m_scale);
		m_quad->Draw();
	glUseProgram(0);
}
