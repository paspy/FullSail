#include "pch.h"
#include "IScene.h"
#include "EntityManager.h"
#include "MainWindow.h"
#include "Game.h"
#include "Camera.h"
//#include "ParticleManager.h"


IScene::IScene(void) : m_MainCamera(nullptr), m_Timer(3.0f), m_FadeIn(false), m_HUD_Off(false)
{
	CGame* game = (CGame*)CGame::GetApplication();


	// scene type
	sceneType = SceneType::UNKNOWN;

	// scene manager
	sceneManager = game->GetSceneManager();

	// init particle manager
	//FSParticle::CParticleManager::GetInstance();

	// setup camera
	m_MainCamera.reset(new Camera);

	// gamepad connection notice
	m_pGamepadNotice = G2D::Factory::CreateGUIText(L"", XMFLOAT2((game->m_window->GetWindowWidth() * 0.5f), game->m_window->GetWindowWidth() *0.02f), 0.0f, 1.f, 0.f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
    reinterpret_cast<G2D::GUIText*>( m_pGamepadNotice )->Layer( 2 );
    reinterpret_cast<G2D::GUIText*>( m_pGamepadNotice )->CenterPrivot( true );
    m_GUIManager.AddEntity(m_pGamepadNotice);
    m_pGamepadNotice->Active( false );
	m_FadeAmount = 0.0f;
}


IScene::~IScene(void)
{
	//IScene::Exit();

	// release 2D stuff
    m_GUIManager.RemoveEntity( m_pGamepadNotice );
    SafeRelease( m_pGamepadNotice );


	// shutdown
	ShutdownScene();
	//FSParticle::CParticleManager::GetInstance()->RemoveAll();

	// null scene manager
	if (sceneManager != nullptr)
		sceneManager = nullptr;
}


void IScene::Enter(void)
{
	// Setup camera

	CGame::GetApplication()->m_pRenderer->SetMainCamera(m_MainCamera->View());

}


void IScene::Exit(void)
{
	GamePad::Get().SetVibration(0, 0, 0);
}


void IScene::Update(float _dt)
{
	



	// update stuff
	m_MainCamera->Update(_dt);
	//FSParticle::CParticleManager::GetInstance()->UpdateAll(_dt, m_MainCamera.get());
	m_GUIManager.UpdateAll(_dt);
	static float scrollSpeed = 0.0f;
	scrollSpeed += 0.005f*_dt;

    CGame::GetApplication()->m_pRenderer->SetWaterScrollSpeed( scrollSpeed );
    CGame::GetApplication()->m_pRenderer->SetDeltaTime( _dt );

	
}


void IScene::Render(void)
{
	//CGame::GetApplication()->m_pRenderer->ClearRenderables();
	//std::vector<Renderer::CRenderable*> renderList = entityManager.GetRenderables(EntityManager::EntityBucket::RENDERABLE);
	//CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	// draw & present
	CGame::GetApplication()->m_pRenderer->Draw();

}


void IScene::DrawGamepadStatus(std::wstring status)
{
	// remove old notice
	//m_GUIManager.RemoveEntity(m_pGamepadNotice);
    m_pGamepadNotice->Active( false );


	// don't draw
	if (status == L"Quit")
		return;

	// setup status text
	status = L"Controller " + status;
	reinterpret_cast<G2D::GUIText*>(m_pGamepadNotice)->Text(status);


	// add new notice
    m_pGamepadNotice->Active( true );
    //m_GUIManager.AddEntity(m_pGamepadNotice);
}


