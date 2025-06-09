#include "scenesplash.h"
#include "sprite.h"
#include "renderer.h"
#include "logmanager.h"

SceneSplash::SceneSplash()
    : m_pAUTSprite(nullptr) 
    , m_pFMODSprite(nullptr)
    , m_alpha(0.0f) 
    , m_timer(0.0f) 
    , m_fadingIn(true)
    , m_finished(false) 
    , m_stage(0) 
{
}

SceneSplash::~SceneSplash()
{
    delete m_pAUTSprite;
    m_pAUTSprite = nullptr;

    delete m_pFMODSprite; 
    m_pFMODSprite = nullptr; 
}

bool SceneSplash::Initialise(Renderer& renderer)
{
    m_pAUTSprite = renderer.CreateSprite("../game/assets/Sprites/AUTLogo.png"); 
    if (m_pAUTSprite) { 
        m_pAUTSprite->SetScale(1.25f, -1.25f);
        m_pAUTSprite->SetAlpha(0.0f);

        int screenW = renderer.GetWidth();
        int screenH = renderer.GetHeight();

        int logoW = m_pAUTSprite->GetWidth();
        int logoH = m_pAUTSprite->GetHeight();

        float scaleX = (screenW / logoW) * 0.5f;
        float scaleY = (screenH / logoH) * 0.5f;

        m_pAUTSprite->SetScale(scaleX, scaleY);
        m_pAUTSprite->SetX(screenW / 2);
        m_pAUTSprite->SetY(screenH / 2);
    }

    m_pFMODSprite = renderer.CreateSprite("../game/assets/Sprites/FMODLogo.png");
    if (m_pFMODSprite) {
        m_pFMODSprite->SetScale(1.25f, -1.25f);
        m_pFMODSprite->SetAlpha(0.0f);

        int screenW = renderer.GetWidth();
        int screenH = renderer.GetHeight();

        int logoW = m_pFMODSprite->GetWidth();
        int logoH = m_pFMODSprite->GetHeight();

        float scaleX = (screenW / logoW) * 0.5f;
        float scaleY = (screenH / logoH) * 0.5f;

        m_pFMODSprite->SetScale(scaleX, scaleY);
        m_pFMODSprite->SetX(screenW / 2);
        m_pFMODSprite->SetY(screenH / 2);
    }

    return true;
}

void SceneSplash::Process(float deltaTime)
{
    m_timer += deltaTime;

    if (m_stage > 1) {
        m_finished = true;
        return;
    }

    Sprite* currentLogo = (m_stage == 0) ? m_pAUTSprite : m_pFMODSprite;
    
    if (!currentLogo) {
        return;
    }

    if (m_fadingIn)
    {
        m_alpha += deltaTime * 0.5f; // 1 second fade-in 
        if (m_alpha >= 1.0f) 
        {
            m_alpha = 1.0f;
            m_fadingIn = false;
            m_timer = 0.0f;
        }
    }
    else
    {
        if (m_timer > 2.0f) // Hold full opacity for 2 seconds
        {
            m_alpha -= deltaTime; // fade out
            if (m_alpha <= 0.0f)
            {
                m_alpha = 0.0f;
                m_stage++;
                m_fadingIn = true;
                m_timer = 0.0f;
            }
        }
    }

    if (m_finished) return;

    if (currentLogo) 
    {
        currentLogo->SetAlpha(m_alpha); 
    }
}

void SceneSplash::Draw(Renderer& renderer)
{
    if (m_stage == 0 && m_pAUTSprite)
    {
        m_pAUTSprite->Draw(renderer); 
    }

    if (m_stage == 1 && m_pFMODSprite)
    {
        m_pFMODSprite->Draw(renderer);  
    }
}

int SceneSplash::GetStage() const {
    return m_stage;
}

bool SceneSplash::IsFinished() const
{
    return m_finished;
}

float SceneSplash::GetAlpha() const
{
    return m_alpha;
}
