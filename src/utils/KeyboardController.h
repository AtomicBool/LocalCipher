#pragma once
#include <windows.h>
#include <initializer_list>

class KeyboardController
{
public:
    static void PressKey(WORD key, bool down)
    {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;

        SendInput(1, &input, sizeof(INPUT));
    }

    static void Combo(std::initializer_list<WORD> keys)
    {
        for (auto k : keys)
            PressKey(k, true);

        Sleep(5);

        for (auto it = keys.end(); it != keys.begin();)
        {
            --it;
            PressKey(*it, false);
        }
    }
};