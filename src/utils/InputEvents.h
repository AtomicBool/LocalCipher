#pragma once
#include <unordered_map>
#include <windows.h>

#include "utils/KeyboardState.h"

class InputEvents
{
public:
    bool IsPressed(int vk, ULONGLONG cooldownMs = 150)
    {
        ULONGLONG now = GetTickCount64();

        if (!KeyboardState::IsDown(vk))
            return false;

        auto& last = m_last[vk];

        if (now - last < cooldownMs)
            return false;

        last = now;
        return true;
    }

private:
    std::unordered_map<int, ULONGLONG> m_last;
};