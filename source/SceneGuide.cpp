#include "sceneguide.h"
#include "sprite.h"
#include "renderer.h"
#include "logmanager.h"
 
SceneGuide::SceneGuide()
    : m_controlGuide(nullptr) 
    , m_alpha(0.0f)
    , m_timer(0.0f)
    , m_fadingIn(true)
    , m_finished(false)
{
}

SceneGuide::~SceneGuide()
{
    delete m_controlGuide;
    m_controlGuide = nullptr;
}

bool SceneGuide::Initialise(Renderer& renderer)
{
    m_controlGuide = renderer.CreateSprite("../game/assets/Sprites/UI/Control-Guide.png");
    if (m_controlGuide) {
        int screenW = renderer.GetWidth();
        int screenH = renderer.GetHeight();

        int logoW = m_controlGuide->GetWidth();
        int logoH = m_controlGuide->GetHeight();

        float scaleX = (screenW / logoW) * 0.5f;
        float scaleY = (screenH / logoH) * 1.0f;

        m_controlGuide->SetScale(scaleX, -scaleY);
        m_controlGuide->SetX(screenW / 2);
        m_controlGuide->SetY(screenH / 2);
        m_controlGuide->SetAlpha(0.0f); 
    }

    return true;
}

void SceneGuide::Process(float deltaTime)
{
    m_timer += deltaTime;

    if (m_finished) return; 

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
        if (m_timer > 7.0f) // Hold full opacity for 7 seconds
        {
            m_alpha -= deltaTime; // fade out
            if (m_alpha <= 0.0f)
            {
                m_alpha = 0.0f;
                m_finished = 0.0f;
            }
        }
    }

    if (m_controlGuide)
    {
        m_controlGuide->SetAlpha(m_alpha);
    }
}

void SceneGuide::Draw(Renderer& renderer)
{
    if (!m_finished && m_controlGuide) 
    {
        m_controlGuide->Draw(renderer); 
    }

}

bool SceneGuide::IsFinished() const
{
    return m_finished;
}

float SceneGuide::GetAlpha() const
{
    return m_alpha;
}
