#pragma once
#include <windows.h>
#include <map>

class Keyboard {
public:
    static Keyboard& Get() {
        static Keyboard instance;
        return instance;
    }

    // with debounce
    bool IsKeyPressed(int vkCode, ULONGLONG interval = 150);

    void simulateCombo(std::initializer_list<WORD> keys);

private:
    Keyboard() = default;
    std::map<int, ULONGLONG> m_lastKeyTime;

    // raw result, without debounce
    bool IsKeyDown(int vkCode);
    void pressKey(WORD key, bool down);
};