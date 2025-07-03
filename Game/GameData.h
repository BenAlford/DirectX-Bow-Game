#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================

#include "GameState.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <list>
#include <vector>
#include "CMOGO.h"
#include "CMOGOSphere.h"
#include "gameobject.h"
#include "GameObject2D.h"
#include "../DirectXTK/Inc/Effects.h"

using namespace DirectX;

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState
	bool m_last_shot_hit = false;

	//player input
	Keyboard::State m_KBS;
	Mouse::State m_MS;
	Keyboard::KeyboardStateTracker m_KBS_tracker;

	std::vector<std::shared_ptr<GameObject>> m_GameObjects; //data structure to hold pointers to the 3D Game Objects
	std::vector<std::shared_ptr<GameObject2D>> m_GameObjects2D; //data structure to hold pointers to the 2D Game Objects 

	std::vector<std::shared_ptr<CMOGO>> m_ColliderObjects;
	std::vector<std::shared_ptr<CMOGOSphere>> m_ColliderSphereObjects;
	std::vector<std::shared_ptr<CMOGO>> m_PhysicsObjects;

	int m_objectNumber = 1;
	bool m_wonGame = false;

	Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
	std::shared_ptr<DirectX::IEffectFactory> m_fxFactory = NULL;
};
#endif
