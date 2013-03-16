//GraphicsApp.cpp
//last modified 05/01/2013

#include "GraphicsApp.h"


GLfloat seaQuadData[] = {
	-1.0f,  0.0f, 1.0f,

	1.0f,  0.0f, 1.0f,
	-1.0f, 0.0f, -1.0f,
	1.0f, 0.0f, -1.0f,
};

GraphicsApp::GraphicsApp(){
	m_forwardsAmount = 0.0f;
	m_sidewaysAmount = 0.0f;
	m_yRotationAmount = 0.0f;
	m_xRotationAmount = 0.0f;
	m_climbAmount = 0.0f;
	m_triangles = true;
	m_winWidth  = APP_WIDTH;
	m_winHeight = APP_HEIGHT;
}

bool GraphicsApp::Init(){

	
	//MAYBE TRY TO SET A RESIZE FUNCTION TO HANDLE RESHAPES?
	glfwSetWindowSizeCallback(Resize);
	glfwSetKeyCallback(KeyCallback);
	glfwSetMousePosCallback(MouseCallback);


	m_reflectionBuffer.InitFramebuffer(m_winWidth,m_winHeight,GL_LINEAR,GL_LINEAR);
	m_atmosphere.InitFramebuffer(m_winWidth,m_winHeight,GL_LINEAR,GL_LINEAR);
	m_shadowBuffer.InitFramebuffer(2048,2048,GL_NEAREST,GL_NEAREST,true);

	m_cam = new Camera(glm::vec3(512.0f,50.0f,-512.0f),glm::vec3(512.0f,50.0f,-517.0f),glm::vec3(0.0f,1.0f,0.0f));

	float aspect = m_winWidth/m_winHeight;
	m_persp = glm::perspective(FOV,aspect,NEAR,FAR);


	//TODO - make sure shader manager has correct creation functions for each


	m_sunAngle = vec2(0.0f, glm::radians(45.0f));

	m_textureManager = new ResourceManager<Texture>(CreateTexture);
	m_shaderManager  = new ResourceManager<Shader>(CreateShaderWithVerts);
	m_meshManager    = new ResourceManager<StaticMesh>(CreateStaticMesh);
	
	InitGeometry();
	InitTextures();
	InitShaders();
	


	m_timer = Timer::GetInstance();

	m_timer->Start();

	
	m_debugView = glm::lookAt(vec3(0.0f,1500.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(1.0f,0.0f,0.0f));
	m_debugPersp = glm::ortho(-600.0f,600.0f,-600.0f,600.0f,0.01f,1500.0f);

	return true;
}

void GraphicsApp::InitTextures(){

	m_waterNormalMap = m_textureManager->Add("normalWater.jpg","textures\\");
	m_grassTex       = m_textureManager->Add("grass-texture-2.jpg","textures\\");
	m_rockTexture    = m_textureManager->Add("rockTexture.png","textures\\");
	m_treeTexture    = m_textureManager->Add("palm_texture.tga","textures\\");
	m_sand           = m_textureManager->Add("sand.jpg","textures\\");
}

void GraphicsApp::InitShaders(){

	m_seaShader     =  m_shaderManager->Add("seaShader","shaders\\");

	m_shaderManager->ChangeResourceCreation(CreateShaderWithVertsNormsTextures);
	m_treeShader    =  m_shaderManager->Add("TexturedMeshShader","shaders\\");
	m_terrainShader =  m_shaderManager->Add("HeightFieldShader","shaders\\");
	m_skyShader     =  m_shaderManager->Add("skyShader","shaders\\");

	Shader* shader = m_shaderManager->GetElement(m_seaShader);

	glUseProgram(shader->GetProgramHandle());
	  shader->SetUniform("boxExtents",m_terrain->GetTree()->GetMinAndMax());
	glUseProgram(0);

}

void GraphicsApp::InitGeometry(){

	m_quad = new Quad();
	m_quad->Init(NULL,true);

	m_terrain = new Terrain("newHeight1.bmp","Textures\\");
	m_terrain->Init(m_textureManager,m_shaderManager,m_meshManager);

	m_sky = new SkyDome(100.0f);
	m_sky->Init("sometexture",m_shaderManager,m_textureManager,m_meshManager);

	//sea

    m_seaQuad = new Quad();
    m_seaQuad->Init(seaQuadData);
}

void GraphicsApp::SetShadowMatrices(){
	vec3 eyePos = m_cam->GetPos();

	vec3 newEye = eyePos - (500.0f * vec3(m_sunVector));

	m_viewMatrix = glm::lookAt(newEye,eyePos,vec3(0.0f,1.0f,0.0f));
}

void  GraphicsApp::PreRender(){

	Frustum* frustum = Frustum::GetInstance();
	vec3 sunPos =  m_cam->GetPos() - vec3(m_sunVector);

	SetShadowMatrices();

	
	m_persp = glm::ortho(-20.0f,20.0f,-20.0f,20.0f,NEAR,FAR);
	glViewport(0,0,2048,2048);
	frustum->Extract(sunPos,m_viewMatrix,m_persp);
	glBindFramebuffer(GL_FRAMEBUFFER,m_shadowBuffer.GetTextureHandle());
	  glFrontFace(GL_CW);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  RenderIsland(false,false,true);
	  glFrontFace(GL_CCW);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,m_winWidth,m_winHeight);


	m_persp = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,NEAR,FAR);
	frustum->Extract(sunPos,m_viewMatrix,m_persp);
	m_sunMatrix = frustum->GetProjectionMatrix() * frustum->GetModelViewMatrix();

	m_viewMatrix = m_cam->GetCamMatrix();
	m_persp = glm::perspectiveFov(FOV,(float)m_winWidth,(float)m_winHeight,NEAR,FAR);
	frustum->Extract(m_cam->GetPos(),m_viewMatrix,m_persp);
	
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER,m_reflectionBuffer.GetTextureHandle());
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  RenderIsland(false,true,false);
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void GraphicsApp::RenderSea(){

	Texture* water = m_textureManager->GetElement(m_waterNormalMap);
	glEnable(GL_BLEND);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,m_reflectionBuffer.GetTextureHandle());
	water->Bind(GL_TEXTURE1);
	Shader* seaShader = m_shaderManager->GetElement(m_seaShader);
	glUseProgram(seaShader->GetProgramHandle());
		seaShader->SetUniform("projMatrix",m_persp);
		seaShader->SetUniform("viewMatrix",m_cam->GetCamMatrix() * glm::scale(FAR*2.0f,1.0f,FAR*2.0f));
		seaShader->SetUniform("winWidth",m_winWidth);
		seaShader->SetUniform("lightPos", m_cam->GetPos() - vec3(m_sunVector));
		seaShader->SetUniform("winHeight",m_winHeight);
		seaShader->SetUniform("reflectionMatrix",m_sunMatrix);
		seaShader->SetUniform("noiseTile", 10.0f);
		seaShader->SetUniform("noiseFactor", 0.1f);
		seaShader->SetUniform("noiseTexture",1);
		seaShader->SetUniform("texWaterReflection",0);
		seaShader->SetUniform("time",Timer::GetInstance()->GetCurrentTime());
		seaShader->SetUniform("waterShininess", 50.0f);
		m_seaQuad->Draw();
	glUseProgram(0);
	water->Unbind(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_BLEND);
}

void GraphicsApp::MainLoop(){
	

		

}

void GraphicsApp::Render(){

	RenderIsland(false,false,false);
	RenderSea();
}

void GraphicsApp::RenderIsland(bool debug, bool reflection,bool depth)
{
	float sun_cosy = cosf(m_sunAngle.y);


	if(!depth){

	  if(reflection){
		  glDisable(GL_CULL_FACE);
		  m_cam->CreateInvertedCam(10.0);
		  m_viewMatrix = glm::scale(-1.0f,1.0f,1.0f) * m_cam->GetInvertedMatrix();
		  Frustum::GetInstance()->Extract(m_cam->GetPos(),m_viewMatrix,m_persp);
	  }else{
         glEnable(GL_CULL_FACE);
		 m_viewMatrix = m_cam->GetCamMatrix();
		 Frustum::GetInstance()->Extract(m_cam->GetPos(),m_viewMatrix,m_persp);
	  }
	}

	if(!depth){

		mat4x4 persp,view;
		if(debug){
			persp = m_debugPersp;
			view = m_debugView;
		}else{
			persp = m_persp;
			view  = m_viewMatrix;
		}

		vec3 pos = m_cam->GetPos();
		m_sky->Render(persp * view * glm::translate(pos.x,pos.y,pos.z) * glm::scale(35.0f,35.0f,35.0f));
	
		Texture* grass = m_textureManager->GetElement(m_grassTex);
		Texture* rock  = m_textureManager->GetElement(m_rockTexture);
		Texture* sand  = m_textureManager->GetElement(m_sand);
		grass->Bind(GL_TEXTURE2);
		rock->Bind(GL_TEXTURE3);
		sand->Bind(GL_TEXTURE4);

		Shader* shader = m_shaderManager->GetElement(m_terrainShader);

		glUseProgram(shader->GetProgramHandle());
			shader->SetUniform("lightingVector",vec3(m_sunVector));
			shader->SetUniform("MVPMatrix",persp * view);
			shader->SetUniform("grassTexture",2);
			shader->SetUniform("rockTexture",3);
			shader->SetUniform("sandTexture",4);
			shader->SetUniform("sunCosY",sun_cosy);
			shader->SetUniform("textureOffset",(float)512/2.0f);
			shader->SetUniform("lightingColor",vec3(1.0f,1.0f,1.0f));
			shader->SetUniform("texScaleWidth",1.0f/512);
			shader->SetUniform("texScaleHeight",1.0f/512);
			m_terrain->DrawGround();
		glUseProgram(0);
	
		sand->Unbind(GL_TEXTURE4);
		rock->Unbind(GL_TEXTURE3);
		grass->Unbind(GL_TEXTURE2);
	}
 
	Texture* tree = m_textureManager->GetElement(m_treeTexture);
	tree->Bind(GL_TEXTURE1);

	Shader* shader = m_shaderManager->GetElement(m_treeShader);
	glUseProgram(shader->GetProgramHandle());
	  shader->SetUniform("lightingVector",vec3(m_sunVector));
	  shader->SetUniform("VPMatrix",m_persp * m_viewMatrix);
	  shader->SetUniform("texture",1);
	  m_terrain->DrawObjects();

	glUseProgram(0);

	tree->Unbind(GL_TEXTURE1);

}



void GraphicsApp::ReadInput(int key,int action){
	//GLFW_PRESS is 1 and GLFW_RELEASE is 0, so we can use this to our advantage

	switch(key){
		case 'W': 
			m_forwardsAmount = action * FORWARDS_SPEED;
			break;
		case 'S': 
			m_forwardsAmount = action * -FORWARDS_SPEED;
			break;
		case 'A':
			m_sidewaysAmount = action * SIDEWAYS_SPEED;
			break;
		case 'D':
			m_sidewaysAmount = action * -SIDEWAYS_SPEED;
			break;
		case GLFW_KEY_LEFT:
			m_yRotationAmount = action * -ROTATE_YAW_SPEED;
			break;
		case GLFW_KEY_RIGHT:
			m_yRotationAmount = action * ROTATE_YAW_SPEED;
			break;
		case GLFW_KEY_UP:
			m_xRotationAmount = action * -ROTATE_PITCH_SPEED;
			break;
		case GLFW_KEY_DOWN:
			m_xRotationAmount = action * ROTATE_PITCH_SPEED;
			break;
		case 'T':
			if(action == GLFW_PRESS)
			  m_triangles = !m_triangles;
			break;
		case 'Y':
			if(action == GLFW_PRESS)
				m_debug = !m_debug;
			break;
	};

}

void GraphicsApp::ReadMouse(int x, int y)
{
	static int prevX,prevY;

	//mouse stuff
	if( glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) ) {
			m_cam->ComboRotate((float)(x-prevX)*1.0f,(float)(y-prevY)*1.0f);
	}
    
   prevX = x;
   prevY = y;
}

void GraphicsApp::Update(double time){
	
	m_cam->CreateCamMatrix();
	Frustum::GetInstance()->Extract(m_cam->GetPos(),m_viewMatrix,m_persp);

	
	if(m_triangles)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_sunVector = vec4(	-cosf(m_sunAngle.x) * sinf(m_sunAngle.y),
						-cosf(m_sunAngle.y),
						-sinf(m_sunAngle.x) * sinf(m_sunAngle.y),
						0.0f );
	

	m_cam->MoveForward(time * m_forwardsAmount);
	m_cam->Strafe(time * m_sidewaysAmount);
	m_cam->RotateYaw(time * m_yRotationAmount);
	m_cam->CreateCamMatrix();
}

void GraphicsApp::WindowChange(int width,int height){
	glViewport(0,0,width,height);

	float aspect = (float)width/(float)height;

	m_winWidth = width;
	m_winHeight = height;

	m_persp = glm::perspective(FOV,aspect,NEAR,FAR);
}

void GraphicsApp::Shutdown(){

	if(m_seaQuad){
		delete m_seaQuad;
	}

	if(m_terrain){
		delete m_terrain;
	}

	glfwTerminate();

	delete m_shaderManager;
	delete m_textureManager;
}

GraphicsApp* GraphicsApp::GetInstance(){
	if(m_instance == NULL){
		m_instance = new GraphicsApp();
		return m_instance;
	}
	
	return m_instance;
}



void GraphicsApp::DestroyInstance(){
	if(m_instance != NULL){
		delete m_instance;
		m_instance = NULL;
	}
}

GraphicsApp* GraphicsApp::m_instance = NULL;


//window resize callback function - called by GLFW everytime the window size is changed

void GLFWCALL Resize(int width,int height){
	GraphicsApp::GetInstance()->WindowChange(width,height);	
}

void GLFWCALL KeyCallback(int key,int action){
	GraphicsApp::GetInstance()->ReadInput(key,action);
}

void GLFWCALL MouseCallback(int x, int y){
	GraphicsApp::GetInstance()->ReadMouse(x,y);
}


