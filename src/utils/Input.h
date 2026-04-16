#pragma once
#include <windows.h>
#include <map>

class Input {
public:
    static Input& Get() {
        static Input instance;
        return instance;
    }

    // 判断按键是否按下（带防抖）
    bool IsKeyPressed(int vkCode, ULONGLONG interval = 150);
    
    // 判断按键当前是否按住（实时状态）
    bool IsKeyDown(int vkCode);

private:
    Input() = default;
    std::map<int, ULONGLONG> m_lastKeyTime;
};
