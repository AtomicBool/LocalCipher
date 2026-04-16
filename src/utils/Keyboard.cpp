#include "utils/Keyboard.h"

bool Keyboard::IsKeyPressed(int vkCode, ULONGLONG interval) {
    ULONGLONG now = GetTickCount64();
    if (IsKeyDown(vkCode)) {
        if (now - m_lastKeyTime[vkCode] > interval) {
            m_lastKeyTime[vkCode] = now;
            return true;
        }
    }
    return false;
}

void Keyboard::simulateCombo(std::initializer_list<WORD> keys) {
    for (WORD key : keys)
        pressKey(key, true);

    for (auto it = std::rbegin(keys); it != std::rend(keys); ++it)
        pressKey(*it, false);
}

bool Keyboard::IsKeyDown(int vkCode) {
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}

void Keyboard::pressKey(WORD key, bool down) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));
}