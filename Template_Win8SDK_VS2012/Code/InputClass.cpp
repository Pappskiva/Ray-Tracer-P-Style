#include "InputClass.h"
#include "Windowsx.h"


InputClass* InputClass::m_instance;

InputClass::InputClass(){}
InputClass::~InputClass(){}
InputClass* InputClass::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new InputClass();
		m_instance->Initialize();
	}
	return m_instance;
}
void InputClass::Initialize()
{
	m_rightMouseButton = new ButtonEvent();
	m_leftMouseButton = new ButtonEvent();
}
void InputClass::Update(UINT p_message, WPARAM p_wParam, LPARAM p_lParam)
{
	// Updates the input for all the keys
	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		switch (p_message)
		{
		case WM_KEYDOWN:
		{
			if (m_keyBoardEvents[i]->GetButton() == p_wParam)
			{
				m_keyBoardEvents[i]->SetState(BE_PRESSED);
			}
			break;
		}

		case WM_KEYUP:
		{
			if (m_keyBoardEvents[i]->GetButton() == p_wParam)
			{
				m_keyBoardEvents[i]->SetState(BE_CLICKED);
			}
			break;
		}
		}
	}
	switch (p_message)
	{
		// Update mouse
	case WM_MOUSEMOVE:
	{
		m_mousePositionX_prev = m_mousePositionX;
		m_mousePositionY_prev = m_mousePositionY;
		m_mousePositionX = GET_X_LPARAM(p_lParam);
		m_mousePositionY = GET_Y_LPARAM(p_lParam);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		m_leftMouseButton->SetState(BE_PRESSED);
		break;
	}

	case WM_LBUTTONUP:
	{
		m_leftMouseButton->SetState(BE_CLICKED);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		m_rightMouseButton->SetState(BE_PRESSED);
		break;
	}

	case WM_RBUTTONUP:
	{
		m_rightMouseButton->SetState(BE_CLICKED);
		break;
	}

	//case WM_CHAR:
	//{
	//	m_lastCharRead = (TCHAR)p_wParam;
	//	break;
	//}
	}
}
void InputClass::Shutdown()
{
	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		delete m_keyBoardEvents[i];
	}
	m_keyBoardEvents.clear();

	delete m_rightMouseButton;
	delete m_leftMouseButton;
	m_rightMouseButton = nullptr;
	m_leftMouseButton = nullptr;


}

void InputClass::RegisterKey(int p_vkey)
{
	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		if (m_keyBoardEvents[i]->GetButton() == p_vkey)
		{
			return;
		}
	}

	KeyBoardButtonEvent *key = new KeyBoardButtonEvent();
	key->Initialize(p_vkey);

	m_keyBoardEvents.push_back(key);
}

bool InputClass::IsKeyClicked(int p_vkey) const
{
	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		if (m_keyBoardEvents[i]->GetButton() == p_vkey)
		{
			return m_keyBoardEvents[i]->IsClicked();
		}
	}

	return false;
}
bool InputClass::IsKeyPressed(int p_vkey) const
{
	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		if (m_keyBoardEvents[i]->GetButton() == p_vkey)
		{
			return m_keyBoardEvents[i]->IsPressed();
		}
	}

	return false;
}

bool InputClass::IsLeftMousePressed() const
{
	return m_leftMouseButton->IsPressed();
}
bool InputClass::IsLeftMouseClicked() const
{
	return m_leftMouseButton->IsClicked();
}
bool InputClass::IsRightMousePressed() const
{
	return m_rightMouseButton->IsPressed();
}
bool InputClass::IsRightMouseClicked() const
{
	return m_rightMouseButton->IsClicked();
}

void InputClass::ClearInput()
{
	if (m_leftMouseButton->IsClicked())
	{
		m_leftMouseButton->SetState(BE_NONE);
	}

	if (m_rightMouseButton->IsClicked())
	{
		m_rightMouseButton->SetState(BE_NONE);
	}

	for (unsigned int i = 0; i < m_keyBoardEvents.size(); i++)
	{
		if (m_keyBoardEvents[i]->IsClicked())
		{
			m_keyBoardEvents[i]->SetState(BE_NONE);
		}
	}
	m_lastCharRead = '\0';
}


bool KeyBoardButtonEvent::Initialize(int p_key)
{
	m_key = p_key;
	m_state = BE_NONE;
	return true;
}
int KeyBoardButtonEvent::GetButton() const
{
	return m_key;
}

bool ButtonEvent::IsPressed() const
{
	if (m_state == BE_PRESSED)
	{
		return true;
	}
	return false;
}
bool ButtonEvent::IsClicked() const
{
	if (m_state == BE_CLICKED)
	{
		return true;
	}
	return false;
}
void ButtonEvent::SetState(BE_State p_state)
{
	m_state = p_state;
}