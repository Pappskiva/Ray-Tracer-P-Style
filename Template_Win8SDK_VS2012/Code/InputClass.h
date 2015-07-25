#pragma once
#include <vector>

typedef _W64 unsigned int UINT_PTR;
typedef long LPARAM;
typedef UINT_PTR WPARAM;
typedef unsigned int UINT;


enum BE_State
{
	BE_NONE,
	BE_PRESSED,
	BE_CLICKED
};
class ButtonEvent
{
public:
	bool IsPressed() const;
	bool IsClicked() const;

	void SetState(BE_State p_state);
protected:
	BE_State m_state;
};

class KeyBoardButtonEvent : public ButtonEvent
{
public:
	bool Initialize(int p_key);
	int GetButton() const;
private:
	int m_key;
};

class InputClass
{
public:
	void Initialize();
	void Update(UINT p_message, WPARAM p_wParam, LPARAM p_lParam);
	void Shutdown();

	void RegisterKey(int p_vkey);

	bool IsKeyClicked(int p_vkey) const;
	bool IsKeyPressed(int p_vkey) const;

	bool IsLeftMousePressed() const;
	bool IsLeftMouseClicked() const;
	bool IsRightMousePressed() const;
	bool IsRightMouseClicked() const;

	void ClearInput();

private:
	std::vector<KeyBoardButtonEvent*> m_keyBoardEvents;

	ButtonEvent *m_leftMouseButton;
	ButtonEvent *m_rightMouseButton;

	char m_lastCharRead;
};
