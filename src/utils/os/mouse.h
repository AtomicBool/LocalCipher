#pragma once

#include <windows.h>

class Mouse
{
public:
    static POINT getPos()
    {
        POINT p{};
        GetCursorPos(&p);
        return p;
    }

    static int getX()
    {
        return getPos().x;
    }

    static int getY()
    {
        return getPos().y;
    }
};