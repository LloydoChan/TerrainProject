//Main.cpp - file holding most includes and main func for the demo.
//GLGraphicsDemos Project - a simple project using GLSL and GLFW to display different scenes and terrains on screen
//now uses my custom opengl api library/engine
//Author: Lloyd Crawley
//Date Last modified: 03/16/2013

//essential includes

#include "GraphicsApp.h"


using namespace std;

GraphicsApp* theApp = NULL;

bool GamePreload()
{
	theApp = GraphicsApp::GetInstance();
	return true;
}

bool GameInit()
{

	if(!theApp->Init()) return false;

	return true;
}

void GameUpdate(double dt)
{
	theApp->Update(dt);
};

void GameRender()
{
	theApp->PreRender();
	theApp->Render();
};

void GameEnd()
{
	theApp->DestroyInstance();
}