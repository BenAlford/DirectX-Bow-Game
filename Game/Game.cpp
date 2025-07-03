//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <time.h>

#include <iostream>
#include <fstream>
#include <sstream>

//Scarle Headers
#include "GameData.h"
#include "GameState.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"

#include "CMOGO.h"
#include <DirectXCollision.h>
#include "Collision.h"
#include "Projectile.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0)
{
    srand(time(nullptr));

    m_level = 1;
    m_maxLevels = 10;
    m_resetTimer = 0;
    m_resetTime = 2;
    m_canReset = true;

    m_startHovered = true;
    m_enterPressed = false;
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND _window, int _width, int _height)
{
    m_window = _window;
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //seed the random number generator
    srand((UINT)time(NULL));

    //set up keyboard and mouse system
    //documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(_window);
    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    //Hide the mouse pointer
    ShowCursor(false);

    //create GameData struct and populate its pointers
    m_GD = new GameData;
    m_GD->m_GS = GS_MAIN_MENU;
    m_GD->m_d3dDevice = m_d3dDevice;

    //set up systems for 2D rendering
    m_DD2D = new DrawData2D;
    m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
    m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"..\\Assets\\italic.spritefont"));
    m_states = std::make_shared<CommonStates>(m_d3dDevice.Get());

    //set up DirectXTK Effects system
    m_fxFactory = std::make_shared<EffectFactory>(m_d3dDevice.Get());
    m_GD->m_fxFactory = m_fxFactory;
    //Tell the fxFactory to look to the correct build directory to pull stuff in from
    (std::static_pointer_cast<EffectFactory>(m_fxFactory))->SetDirectory(L"..\\Assets");
    //init render system for VBGOs
    VBGO::Init(m_d3dDevice.Get());

    //set audio system
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);

    //find how big my window is to correctly calculate my aspect ratio
    m_aspectRatio = (float)_width / (float)_height;
    m_width = _width;
    m_height = _height;

    //create DrawData struct and populate its pointers
    m_DD = new DrawData;
    m_DD->m_pd3dImmediateContext = nullptr;

    // loads the main menu
    LoadMainMenu();

    ////Test Sounds
    ////Loop* loop = new Loop(m_audioEngine.get(), "NightAmbienceSimple_02");
    ////loop->SetVolume(0.1f);
    ////loop->Play();
    ////m_Sounds.push_back(loop);

    //TestSound* TS = new TestSound(m_audioEngine.get(), "Explo1");
    //m_Sounds.push_back(TS);
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        if (m_enterPressed && !m_GD->m_KBS_tracker.pressed.Enter)
        {
            m_enterPressed = false;
        }

        Update(m_timer);
        if (m_GD->m_GS == GS_MAIN_MENU)
            UpdateMenu(m_timer);

        // moves from game over menu to main menu if enter if pressed
        if (m_GD->m_GS == GS_GAME_OVER)
        {
            if (m_GD->m_KBS_tracker.pressed.Enter)
            {
                m_GD->m_GS = GS_MAIN_MENU;
                LoadMainMenu();
            }
        }
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& _timer)
{
    float elapsedTime = float(_timer.GetElapsedSeconds());

    // makes the game slow down if frame rate drops below 60, otherwise collisions would break
    if (elapsedTime > 1.f / 60.f)
    {
        elapsedTime = 1.f / 60.f;
    }
    m_GD->m_dt = elapsedTime;

    //this will update the audio engine but give us chance to do somehting else if that isn't working
    if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            // We lost the audio device!
        }
    }
    else
    {
        //update sounds playing
        for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
        {
            (*it)->Tick(m_GD);
        }
    }

    ReadInput();

    // makes it so you can't spam reset by having a cooldown on it
    if (!m_canReset)
    {
        m_resetTimer += m_GD->m_dt;
        if (m_resetTimer > m_resetTime)
        {
            m_resetTimer = 0;
            m_canReset = true;
        }
    }

    // resets the level when R is pressed
    if (m_GD->m_GS == GS_PLAY_TPS_CAM && m_canReset && m_GD->m_KBS_tracker.pressed.R)
    {
        m_canReset = false;
        // correctly sets the shots
        int shots = m_player->getShots();
        LoadLevel(m_level);
        int max_shots = m_player->getShots();
        m_player->setShots(shots);

        for (int i = 0; i < max_shots - shots; i++)
        {
            m_GD->m_GameObjects2D.pop_back();
        }
    }

    //update all objects
    for (int i = 0; i < m_GD->m_GameObjects.size(); i++)
    {
        m_GD->m_GameObjects[i]->Tick(m_GD);
    }
    for (auto& object2D : m_GD->m_GameObjects2D)
    {
        object2D->Tick(m_GD);
    }

    CheckCollision();

    // calls late tick on all objects to do post collision / post movement code execution
    for (auto& object : m_GD->m_GameObjects)
    {
        object->LateTick(m_GD);
    }

    // precedes to next level when the level is won
    if (m_GD->m_GS == GS_LEVEL_WON)
    {
        // makes a camera that zooms into the target to show a close up of the shot
        m_GoToCam = std::make_shared<GoToCamera>(0.25f * XM_PI, m_aspectRatio, 1.0f, 10000.0f, m_target, Vector3::UnitY, m_TPScam->GetPos());
        m_GoToCam->SetID(m_GD->m_objectNumber);
        m_GD->m_objectNumber++;
        m_GD->m_GameObjects.push_back(m_GoToCam);

        m_GD->m_GameObjects2D.clear();

        // displays text
        std::shared_ptr<TextGO2D> continue_text = std::make_shared<TextGO2D>("Level Won! Press Enter to continue");
        continue_text->SetPos(Vector2((m_width / 2) - 300, m_height - 300));
        m_GD->m_GameObjects2D.push_back(continue_text);

        m_DD->m_cam = m_GoToCam;
        m_GD->m_GS = GS_WAITING_TO_CONTINUE;
    }

    // shows the lose screen
    else if (m_GD->m_GS != GS_WAITING_TO_CONTINUE && m_GD->m_last_shot_hit)
    {
        m_GD->m_GS = GS_GAME_OVER;
        LoadGameOver();
    }

    // if enter is pressed when the level is won, go to next level
    if (m_GD->m_GS == GS_WAITING_TO_CONTINUE && !m_enterPressed && m_GD->m_KBS_tracker.pressed.Enter)
    {
        m_enterPressed = true;
        m_level++;

        // if there is another level
        if (m_level <= m_maxLevels)
        {
            // load it
            LoadLevel(m_level);
            m_GD->m_GS = GS_PLAY_TPS_CAM;

            // if this is the last level (playground) set the game to won so its unlosable
            if (m_level == m_maxLevels)
            {
                LoadGameWon();
            }
        }
        // if there are no more levels, return to menu
        else
        {
            m_GD->m_GS = GS_MAIN_MENU;
            LoadMainMenu();
        }
    }
}

void Game::UpdateMenu(DX::StepTimer const& _timer)
{
    // if enter is pressed, an option has been chosen
    if (!m_enterPressed && m_GD->m_KBS_tracker.pressed.Enter)
    {
        m_enterPressed = true;

        // if start was pressed, load the first level
        if (m_startHovered)
        {
            m_GD->m_GS = GS_PLAY_TPS_CAM;
            m_GD->m_wonGame = false;
            m_level = 1;
            LoadLevel(m_level);
        }
        // else quit
        else
        {
            ExitGame();
        }
    }

    // switches to start if a is pressed and is not selected
    else if (m_GD->m_KBS.A && !m_startHovered && !m_GD->m_KBS.D)
    {
        m_startText->SetText("-> Start");
        m_quitText->SetText("   Quit");
        m_startHovered = true;
    }
    // switches to quit if d was pressed and is not selected
    else if (m_GD->m_KBS.D && m_startHovered && !m_GD->m_KBS.A)
    {
        m_startText->SetText("   Start");
        m_quitText->SetText("-> Quit");
        m_startHovered = false;
    }
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();
    
    //set immediate context of the graphics device
    m_DD->m_pd3dImmediateContext = m_d3dContext.Get();

    //set which camera to be used
    //m_DD->m_cam = m_cam;
    if (m_GD->m_GS == GS_PLAY_TPS_CAM)
    {
        m_DD->m_cam = m_TPScam;
    }

    //update the constant buffer for the rendering of VBGOs
    VBGO::UpdateConstantBuffer(m_DD);

    //Draw 3D Game Obejects
    for (auto& object : m_GD->m_GameObjects)
    {
        object->Draw(m_DD);
    }

    // Draw sprite batch stuff 
    m_DD2D->m_Sprites->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());
    for (auto& object2D : m_GD->m_GameObjects2D)
    {
        object2D->Draw(m_DD2D);
    }
    m_DD2D->m_Sprites->End();

    //drawing text screws up the Depth Stencil State, this puts it back again!
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    float a[4] = { 0.7294117647 ,0.8470588235 , 0.8784313725 , 1};
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), a);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);
    
    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int _width, int _height)
{
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& _width, int& _height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    _width = 1080;
    _height = 920;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    //creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    //something missing on the machines in 2Q28
    //this should work!
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}

void Game::LoadLevel(int level)
{
    // clears all vectors and resets variables
    m_GD->m_last_shot_hit = false;
    m_GD->m_objectNumber = 1;
    m_GD->m_GameObjects.clear();
    m_GD->m_GameObjects2D.clear();
    m_GD->m_ColliderObjects.clear();
    m_GD->m_ColliderSphereObjects.clear();
    m_GD->m_PhysicsObjects.clear();

    m_GoToCam.reset();

    //create a base light
    m_light = std::make_shared<Light>(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
    m_light->SetID(m_GD->m_objectNumber);
    m_GD->m_objectNumber++;
    m_GD->m_GameObjects.push_back(m_light);

    //create a base camera
    m_cam = std::make_shared<Camera>(0.25f * XM_PI, m_aspectRatio, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
    m_cam->SetID(m_GD->m_objectNumber);
    m_GD->m_objectNumber++;
    m_cam->SetPos(Vector3(0.0f, 200.0f, 200.0f));
    m_GD->m_GameObjects.push_back(m_cam);

    m_DD->m_states = m_states;
    m_DD->m_cam = m_cam;
    m_DD->m_light = m_light;


    // opens the corresponding level file
    ifstream level_file;
    level_file.open("../Levels/" + std::to_string(level) + ".txt");

    int shots;
    level_file >> shots;

    // adds the player
    std::shared_ptr<Player> pPlayer = std::make_shared<Player>("BirdModelV1", m_d3dDevice.Get(), m_fxFactory, shots, Vector3(0, 2, 0));
    m_player = pPlayer;
    pPlayer->SetID(m_GD->m_objectNumber);
    m_GD->m_objectNumber++;
    pPlayer->SetTag("Player");
    pPlayer->SetScale(Vector3(0.5, 3, 0.5));
    m_GD->m_GameObjects.push_back(pPlayer);
    m_GD->m_PhysicsObjects.push_back(pPlayer);

    //add a secondary camera
    m_TPScam = std::make_shared<TPSCamera>(0.25f * XM_PI, m_aspectRatio, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 0.0f, 0.01f));
    m_TPScam->SetID(m_GD->m_objectNumber);
    m_GD->m_objectNumber++;
    m_GD->m_GameObjects.push_back(m_TPScam);

    // gives the player access to the camera (to direct arrows)
    pPlayer->setCamera(m_TPScam);

    // adds bow reticle
    std::shared_ptr<ImageGO2D> bowUI1 = std::make_shared<ImageGO2D>("BowUI1", m_d3dDevice.Get());
    bowUI1->SetPos(Vector2(m_width / 2, m_height / 2));
    m_GD->m_GameObjects2D.push_back(bowUI1);

    // adds the moving part of the reticle
    std::shared_ptr<BowUI> bowUIMoving = std::make_shared<BowUI>("BowUIMoving", m_d3dDevice.Get(), 1.5);
    bowUIMoving->SetPos(Vector2(m_width / 2, m_height / 2));
    m_GD->m_GameObjects2D.push_back(bowUIMoving);

    pPlayer->setBowUI(bowUIMoving);

    // adds all the shot UI indicators
    for (int i = 0; i < shots; i++)
    {
        std::shared_ptr<ImageGO2D> arrow = std::make_shared<ImageGO2D>("arrow2", m_d3dDevice.Get());
        arrow->SetPos(Vector2(32 + (i * 64), 32));
        m_GD->m_GameObjects2D.push_back(arrow);
        pPlayer->addArrowUI(arrow);
    }

    // adds all the level specific game objects
    while (!level_file.eof())
    {
        std::string type, temp;
        level_file >> type;

        // stops false objects being read
        if (type != "")
        {
            // reads shared attributes
            level_file >> temp;
            float restitution = std::stof(temp);

            level_file >> temp;
            float mass = std::stof(temp);

            level_file >> temp;
            float x = std::stof(temp);

            level_file >> temp;
            float y = std::stof(temp);

            level_file >> temp;
            float z = std::stof(temp);

            if (type != "Blackhole")
            {
                // reads none blackhole shared attributes
                float roll, pitch, yaw, scalex, scaley, scalez;

                // converts all angles to radians
                level_file >> temp;
                pitch = std::stoi(temp) * (XM_PI / 180);

                level_file >> temp;
                yaw = std::stoi(temp) * (XM_PI / 180);

                level_file >> temp;
                roll = std::stoi(temp) * (XM_PI / 180);

                level_file >> temp;
                scalex = std::stof(temp);

                level_file >> temp;
                scaley = std::stof(temp);

                level_file >> temp;
                scalez = std::stof(temp);

                // sets up the object depending on its type, reading objects specific attributes where neccessary
                if (type == "Terrain")
                {
                    level_file >> temp;

                    std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>(temp, m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex,scaley,scalez));
                    terrain->SetID(m_GD->m_objectNumber);
                    terrain->SetRestitution(restitution);
                    terrain->SetMass(mass);
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(terrain);
                    m_GD->m_ColliderObjects.push_back(terrain);
                }
                else if (type == "Target")
                {
                    m_target = std::make_shared<Target>("Target_5", m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez));
                    m_target->SetID(m_GD->m_objectNumber);
                    m_target->SetRestitution(restitution);
                    m_target->SetMass(mass);
                    m_target->SetTag("Target");
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(m_target);
                    m_GD->m_ColliderObjects.push_back(m_target);
                }
                else if (type == "PhysicsCube")
                {
                    std::shared_ptr<PhysicsCube> phys = std::make_shared<PhysicsCube>("Crate_4", m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez) * 0.1f);
                    phys->SetID(m_GD->m_objectNumber);
                    phys->SetTag("cube");
                    phys->SetRestitution(restitution);
                    phys->SetMass(mass);
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(phys);
                    m_GD->m_PhysicsObjects.push_back(phys);
                }
                else if (type == "Button")
                {
                    level_file >> temp;
                    float target = std::stoi(temp);
                        
                    level_file >> temp;
                    float x_move = std::stof(temp);
                    level_file >> temp;
                    float y_move = std::stof(temp);
                    level_file >> temp;
                    float z_move = std::stof(temp);

                    std::shared_ptr<Button> button = std::make_shared<Button>("Button_2", m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez), m_GD->m_GameObjects[target + 3], Vector3(x_move,y_move,z_move), 2);
                    button->SetID(m_GD->m_objectNumber);
                    button->SetRestitution(restitution);
                    button->SetMass(mass);
                    button->SetTag("Button");
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(button);
                    m_GD->m_ColliderObjects.push_back(button);
                }
                else if (type == "Spinner")
                {
                    level_file >> temp;
                    float speed = std::stof(temp);

                    level_file >> temp;
                    std::string file_name = temp;

                    std::shared_ptr<Spinner> spin = std::make_shared<Spinner>(file_name, m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez), speed);
                    spin->SetID(m_GD->m_objectNumber);
                    spin->SetRestitution(restitution);
                    spin->SetMass(mass);
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(spin);
                    m_GD->m_ColliderObjects.push_back(spin);
                }
                else if (type == "Spring")
                {
                    level_file >> temp;
                    float m_height = std::stof(temp);
                    std::shared_ptr<Spring> spring = std::make_shared<Spring>("spring2", m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez), m_height);
                    spring->SetID(m_GD->m_objectNumber);
                    spring->SetRestitution(restitution);
                    spring->SetMass(mass);
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(spring);
                    m_GD->m_ColliderObjects.push_back(spring);
                }
                else if (type == "Uncollidable")
                {
                    level_file >> temp;

                    std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>(temp, m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), pitch, yaw, roll, Vector3(scalex, scaley, scalez));
                    terrain->SetID(m_GD->m_objectNumber);
                    terrain->SetRestitution(restitution);
                    terrain->SetMass(mass);
                    m_GD->m_objectNumber++;
                    m_GD->m_GameObjects.push_back(terrain);
                }
            }
            else
            {
                // sets up blackhole
                float influence, strength;

                level_file >> temp;
                influence = std::stoi(temp);

                level_file >> temp;
                strength = std::stoi(temp);

                std::shared_ptr<BlackHole> blackhole = std::make_unique<BlackHole>("New_Black_Hole_3", m_d3dDevice.Get(), m_fxFactory, Vector3(x, y, z), influence, strength);
                blackhole->setIsTrigger(true);
                blackhole->SetID(m_GD->m_objectNumber);
                blackhole->SetRestitution(restitution);
                blackhole->SetMass(mass);
                m_GD->m_objectNumber++;
                m_GD->m_GameObjects.push_back(blackhole);
                m_GD->m_ColliderSphereObjects.push_back(blackhole);
            }

            // sets objects parent if applicable
            level_file >> temp;
            float parent = std::stoi(temp);
            if (parent != 0)
            {
                m_GD->m_GameObjects.back()->SetParent(m_GD->m_GameObjects[parent + 3]);
            }
        }
    }
    // closes file
    level_file.close();

    // creates the background
    std::shared_ptr<Terrain> bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(0, -20, -450), 0, -XM_PI / 2, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(100, -20, -300), 0, XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(-150, -20, -350), 0, 0, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(200, -20, -200), 0, -XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(-200, -20, -150), 0, XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(200, -20, 100), 0, -XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(-200, -20, 150), 0, XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(0, -20, 450), 0, -XM_PI / 2, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(100, -20, 300), 0, XM_PI, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);

    bg = std::make_shared<Terrain>("Terrain", m_d3dDevice.Get(), m_fxFactory, Vector3(-150, -20, 350), 0, 0, 0, Vector3::One);
    m_GD->m_GameObjects.push_back(bg);
}

void Game::LoadMainMenu()
{
    // chooses a random level to load as the menu background
    int rand_level = (std::rand() % (m_maxLevels - 1)) + 1;
    LoadLevel(rand_level);
    m_GD->m_GameObjects2D.clear();

    // sets the selected option to be start
    m_startHovered = true;

    // creates the camera that rotates around the level
    std::shared_ptr<LevelViewCam> level_cam = std::make_shared<LevelViewCam>(0.25f * XM_PI, m_aspectRatio, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero, XM_PI / 18, Vector3(0,40,100));
    m_GD->m_GameObjects.push_back(level_cam);
    m_DD->m_cam = level_cam;

    // creates the menu
    std::shared_ptr<TextGO2D> title = std::make_shared<TextGO2D>("Target Practice");
    title->SetPos(Vector2(120, m_height / 2 - 200));
    title->SetScale(Vector2(3, 3));
    m_GD->m_GameObjects2D.push_back(title);

    m_startText = std::make_shared<TextGO2D>("-> Start");
    m_startText->SetPos(Vector2(m_width / 4 - 100, m_height / 2 + 30));
    m_startText->SetScale(Vector2(2, 2));
    m_GD->m_GameObjects2D.push_back(m_startText);

    m_quitText = std::make_shared<TextGO2D>("   Quit");
    m_quitText->SetPos(Vector2(3 * m_width / 4 - 100, m_height / 2 + 30));
    m_quitText->SetScale(Vector2(2, 2));
    m_GD->m_GameObjects2D.push_back(m_quitText);

    std::shared_ptr<TextGO2D> info = std::make_shared<TextGO2D>("WASD to move, Left click to charge shot, Space to jump,\n Enter to accept, R to reset level");
    info->SetPos(Vector2(0, m_height - 150));
    m_GD->m_GameObjects2D.push_back(info);
}

void Game::LoadGameOver()
{
    // shows the player how to return to the menu
    m_GD->m_GameObjects2D.clear();

    std::shared_ptr<TextGO2D> info = std::make_shared<TextGO2D>("Game Over! Press Enter to return to menu.");
    info->SetPos(Vector2(170, m_height - 300));
    m_GD->m_GameObjects2D.push_back(info);
}

void Game::LoadGameWon()
{
    // Tells the player how to leave the end and tells everything the game has been beaten
    m_beatenGame = true;
    std::shared_ptr<TextGO2D> info = std::make_shared<TextGO2D>("Congratulations you have won! You have no shot\nlimit here. Shoot the target to return to the main menu.");
    info->SetPos(Vector2(0, m_height / 2 + 100));
    m_GD->m_GameObjects2D.push_back(info);
    m_GD->m_wonGame = true;
}

void Game::ReadInput()
{
    m_GD->m_KBS = m_keyboard->GetState();
    m_GD->m_KBS_tracker.Update(m_GD->m_KBS);
    //quit game on hiting escape
    if (m_GD->m_KBS.Escape)
    {
        ExitGame();
    }

    m_GD->m_MS = m_mouse->GetState();

    //lock the cursor to the centre of the window
    RECT window;
    GetWindowRect(m_window, &window);
    SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);
}

void Game::CheckCollision()
{
    // every collision type check has 2 possibilities: neither are triggers or at least one is a trigger

    // checks collisions between physics objects
    for (int i = 0; i < m_GD->m_PhysicsObjects.size() - 1; i++) for (int j = 0; j < m_GD->m_PhysicsObjects.size() - i - 1; j++)
    {
        int size = m_GD->m_PhysicsObjects.size() - 1;
        BoundingOrientedBox c1;
        m_GD->m_PhysicsObjects[i]->getCollider().Transform(c1, m_GD->m_PhysicsObjects[i]->getWorldTransform());

        BoundingOrientedBox c2;
        m_GD->m_PhysicsObjects[size - j]->getCollider().Transform(c2, m_GD->m_PhysicsObjects[size - j]->getWorldTransform());

        if (c1.Intersects(c2))
        {
            // if neither are triggers apply collision resolution between 2 physics objects
            if (!m_GD->m_PhysicsObjects[i]->isTrigger() && !m_GD->m_PhysicsObjects[size - j]->isTrigger())
            {
                Collision::PhysicsToPhysics(*m_GD->m_PhysicsObjects[i], *m_GD->m_PhysicsObjects[size - j]);
            }
            // else call trigger entered for both
            else
            {
                m_GD->m_PhysicsObjects[size - j]->TriggerEntered(m_GD->m_PhysicsObjects[i]);
                m_GD->m_PhysicsObjects[i]->TriggerEntered(m_GD->m_PhysicsObjects[size - j]);
            }
        }
    }

    // checks collisions between physics objects and collider objects
    for (int i = 0; i < m_GD->m_PhysicsObjects.size(); i++) for (int j = 0; j < m_GD->m_ColliderObjects.size(); j++)
    {
        BoundingOrientedBox c1;
        m_GD->m_PhysicsObjects[i]->getCollider().Transform(c1, m_GD->m_PhysicsObjects[i]->getWorldTransform());

        BoundingOrientedBox c2;
        m_GD->m_ColliderObjects[j]->getCollider().Transform(c2, m_GD->m_ColliderObjects[j]->getWorldTransform());

        if (c1.Intersects(c2))
        {
            // if neither are triggers eject the physics object (the hit function is called in collision.h)
            if (!m_GD->m_PhysicsObjects[i]->isTrigger() && !m_GD->m_ColliderObjects[j]->isTrigger())
            {
                XMFLOAT3 eject_vect = Collision::ejectionCMOGO(*m_GD->m_PhysicsObjects[i], *m_GD->m_ColliderObjects[j]);
                auto pos = m_GD->m_PhysicsObjects[i]->GetPos();
                m_GD->m_PhysicsObjects[i]->SetPos(pos - eject_vect);
            }
            else
            {
                m_GD->m_ColliderObjects[j]->TriggerEntered(m_GD->m_PhysicsObjects[i]);
                m_GD->m_PhysicsObjects[i]->TriggerEntered(m_GD->m_ColliderObjects[j]);
            }
        }
    }


    // checks for collision between physics objects and sphere collider objects (black holes)
    for (int i = 0; i < m_GD->m_PhysicsObjects.size(); i++) for (int j = 0; j < m_GD->m_ColliderSphereObjects.size(); j++)
    {
        BoundingOrientedBox c1;
        m_GD->m_PhysicsObjects[i]->getCollider().Transform(c1, m_GD->m_PhysicsObjects[i]->getWorldTransform());

        BoundingSphere c2;
        m_GD->m_ColliderSphereObjects[j]->getSphereCollider().Transform(c2, m_GD->m_ColliderSphereObjects[j]->getWorldTransform());
        if (c1.Intersects(c2)) //std::cout << "Collision Detected!" << std::endl;
        {
            if (!m_GD->m_PhysicsObjects[i]->isTrigger() && !m_GD->m_ColliderSphereObjects[j]->isTrigger())
            {
                // this never happens as all blackholes are triggers
            }
            else
            {
                m_GD->m_PhysicsObjects[i]->TriggerEntered(m_GD->m_ColliderSphereObjects[j]);
                m_GD->m_ColliderSphereObjects[j]->TriggerEntered(m_GD->m_PhysicsObjects[i]);
            }
        }
    }
}