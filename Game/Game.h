//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include <list>
#include "CommonStates.h"
#include "../DirectXTK/Inc/Effects.h" //this clashes with a SDK file so must explitily state it
#include "Keyboard.h"
#include "Mouse.h"
#include "Audio.h"
#include "CMOGOSphere.h"
#include "CMOGO.h"
#include "Player.h"
#include "GoToCamera.h"
#include "TextGO2D.h"

using std::list;

// Forward declarations
struct GameData;
struct DrawData;
struct DrawData2D;
class GameObject;
class GameObject2D;
class Camera;
class TPSCamera;
class Light;
class Sound;
class Terrain;
class Target;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND _window, int _width, int _height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int _width, int _height);

    // Properties
    void GetDefaultSize( int& _width, int& _height ) const noexcept;

private:

    void Update(DX::StepTimer const& _timer);
    void UpdateMenu(DX::StepTimer const& _timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    void LoadLevel(int level);
    void LoadMainMenu();
    void LoadGameOver();
    void LoadGameWon();

    bool m_beatenGame = false;

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

    //Scarle Added stuff
    GameData* m_GD = NULL;			//Data to be shared to all Game Objects as they are ticked
    DrawData* m_DD = NULL;			//Data to be shared to all 3D Game Objects as they are drawn
    DrawData2D* m_DD2D = NULL;	    //Data to be passed by game to all 2D Game Objects via Draw 

    //Basic 3D renderers
    std::shared_ptr<Camera> m_cam = NULL; //principle camera
    std::shared_ptr<TPSCamera> m_TPScam = NULL;//TPS cam
    std::shared_ptr<Light> m_light = NULL; //base light
    std::shared_ptr<Player> m_player = NULL;
    std::shared_ptr<GoToCamera> m_GoToCam = NULL;
    std::shared_ptr<Target> m_target = NULL;

    //required for the CMO model rendering system
    std::shared_ptr<DirectX::CommonStates> m_states = NULL;
    std::shared_ptr<DirectX::IEffectFactory> m_fxFactory = NULL;

    //basic keyboard and mouse input system
    void ReadInput(); //Get current Mouse and Keyboard states
    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;

    void CheckCollision();
                                         
    //sound stuff
	//This uses a simple system, but a better pipeline can be used using Wave Banks
	//See here: https://github.com/Microsoft/DirectXTK/wiki/Creating-and-playing-sounds Using wave banks Section
    std::unique_ptr<DirectX::AudioEngine> m_audioEngine;
    list<Sound*>m_Sounds;

    float m_aspectRatio = 0;
    float m_width = 0;
    float m_height = 0;
    int m_level;
    int m_maxLevels;

    float m_resetTimer;
    float m_resetTime;
    bool m_canReset;

    std::shared_ptr<TextGO2D> m_startText = nullptr;
    std::shared_ptr<TextGO2D> m_quitText = nullptr;
    bool m_startHovered;

    bool m_enterPressed;
};
