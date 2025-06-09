// COMP710 GP Framework 2022
// This include:
#include "inputsystem.h"
// Local includes:
#include "renderer.h"
#include "logmanager.h"
#include "scene.h"
#include "game.h"
#include "xboxcontroller.h"
#include "vector2.h"
// #include "imgui/imgui.h"
// #include "imgui/imgui_impl_sdl2.h"  
// Library includes:
#include <cassert>
#include <SDL.h>

InputSystem::InputSystem()
	: m_pCurrentKeyboardState(0)
	, m_previousMouseButtons(0)
	, m_currentMouseButtons(0)
	, m_pXboxController(0)
	, m_iNumAttachedControllers(0)
	, m_bRelativeMouseMode(false)
{
}

InputSystem::~InputSystem()
{
	delete[] m_pXboxController;
	m_pXboxController = nullptr;
}

bool InputSystem::Initialise()
{
	for (int k = 0; k < SDL_NUM_SCANCODES; ++k)
	{
		m_previousKeyboardState[k] = 0;
		m_currentKeyboardStateCopy[k] = 0;
	}
	m_pCurrentKeyboardState = m_currentKeyboardStateCopy;

	if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER))
	{
		SDL_Init(SDL_INIT_GAMECONTROLLER);
		LogManager::GetInstance().Log("SDL Init Game Controller System");
	}

	ShowMouseCursor(false);

	m_iNumAttachedControllers = SDL_NumJoysticks();
	m_pXboxController = new XboxController[m_iNumAttachedControllers];
	for (int k = 0; k < m_iNumAttachedControllers; ++k)
	{
		if (SDL_IsGameController(k))
		{
			m_pXboxController[k].Initialise(k);
		}
	}
	return true;
}

void InputSystem::ProcessInput()
{
	const Uint8* state = SDL_GetKeyboardState(nullptr);

	for (int k = 0; k < SDL_NUM_SCANCODES; ++k)
	{
		m_previousKeyboardState[k] = m_currentKeyboardStateCopy[k];
		m_currentKeyboardStateCopy[k] = state[k];
	}
	m_pCurrentKeyboardState = m_currentKeyboardStateCopy;

	m_previousMouseButtons = m_currentMouseButtons;
	m_mouseWheel.Set(0.0f, 0.0f);

	int mouseX = 0, mouseY = 0;
	if (m_bRelativeMouseMode)
		m_currentMouseButtons = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	else
		m_currentMouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

	m_mousePosition.Set(static_cast<float>(mouseX), static_cast<float>(mouseY));

	for (int k = 0; k < m_iNumAttachedControllers; ++k)
		m_pXboxController[k].ProcessInput();

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		// ImGuiIO& io = ImGui::GetIO();
		// io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		// if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		// {
		//     ImGui_ImplSDL2_ProcessEvent(&event);
		//     continue;
		// }

		ProcessMouseWheel(event);

		if (event.type == SDL_QUIT)
		{
			Game::GetInstance().Quit();
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE)
		{
			Game::GetInstance().ToggleDebugWindow();
		}
	}
}

void InputSystem::ShowMouseCursor(bool show)
{
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void InputSystem::SetRelativeMode(bool relative)
{
	m_bRelativeMouseMode = relative;
	SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
}

void InputSystem::ProcessMouseWheel(SDL_Event& event)
{
	if (event.type == SDL_MOUSEWHEEL)
	{
		m_mouseWheel.Set(static_cast<float>(event.wheel.x), static_cast<float>(event.wheel.y));
	}
}

ButtonState InputSystem::GetKeyState(SDL_Scancode key)
{
	if (m_previousKeyboardState[key] == 0)
	{
		if (m_currentKeyboardStateCopy[key] == 0)
		{
			return BS_NEUTRAL;
		}
		else
		{
			return BS_PRESSED;
		}
	}
	else
	{
		if (m_currentKeyboardStateCopy[key] == 0)
		{
			return BS_RELEASED;
		}
		else
		{
			return BS_HELD;
		}
	}
}

const Vector2& InputSystem::GetMousePosition() const
{
	return m_mousePosition;
}

const Vector2& InputSystem::GetMouseScrollWheel() const
{
	return m_mouseWheel;
}

ButtonState InputSystem::GetMouseButtonState(int button)
{
	unsigned int mask = 1 << button;
	
	if ((m_previousMouseButtons & mask) == 0)
	{
		if ((m_currentMouseButtons & mask) == 0)
		{
			return BS_NEUTRAL;
		}
		else
		{
			return BS_PRESSED;
		}
	}
	else
	{
		if ((m_currentMouseButtons & mask) == 0)
		{
			return BS_RELEASED;
		}
		else
		{
			return BS_HELD;
		}
	}
}

int InputSystem::GetNumberOfControllersAttached() const
{
	return m_iNumAttachedControllers;
}

XboxController* InputSystem::GetController(int controllerIndex)
{
	if (controllerIndex >= 0 && controllerIndex < m_iNumAttachedControllers)
	{
		return &m_pXboxController[controllerIndex];
	}
	return nullptr;
}