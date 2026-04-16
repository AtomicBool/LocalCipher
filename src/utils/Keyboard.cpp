#include "utils/Keyboard.h"

bool Input::IsKeyPressed(int vkCode, ULONGLONG interval) {
    ULONGLONG now = GetTickCount64();
    if (IsKeyDown(vkCode)) {
        if (now - m_lastKeyTime[vkCode] > interval) {
            m_lastKeyTime[vkCode] = now;
            return true;
        }
    }
    return false;
}

bool Input::IsKeyDown(int vkCode) {
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}
