#pragma once
#include <windows.h>

class KeyboardState
{
public:
    static bool IsDown(int vk)
    {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }
};