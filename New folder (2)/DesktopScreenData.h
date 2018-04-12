#pragma once

#include "atm.h"
#include <windows.h>

class DesktopScreenData {

public:
    HDC handle;

    int width;
    int height;
    int bpp;

    DesktopScreenData() {
        GetDesktopData();
    }

    void GetDesktopData() {
        handle = CreateDCW(L"DISPLAY", L"\\\\.\\DISPLAY1", NULL, NULL);
        width = GetDeviceCaps(handle, HORZRES);
        height = GetDeviceCaps(handle, VERTRES);
        bpp = 32;
    }

    int GetStride() {
        return((((width * bpp)+31)&~31)>>3);
    }

};