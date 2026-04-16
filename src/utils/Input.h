#pragma once
#include <windows.h>
#include <map>

class Input {
public:
    static Input& Get() {
        static Input instance;
        return instance;
    }

    // with debounce
    bool IsKeyPressed(int vkCode, ULONGLONG interval = 150);
    
    // raw result, without debounce
    bool IsKeyDown(int vkCode);

private:
    Input() = default;
    std::map<int, ULONGLONG> m_lastKeyTime;
};
