#pragma once

#include <windows.h>
#include "CaptureData.h"
#include "DesktopScreenData.h"
#include "CaptureBlock.h"

struct TargetDimensions {
    int width;
    int height;
};

class DesktopCapture {
public:
    DesktopCapture(int targetWidth, int targetHeight, int bitCount);

    void Init();
    void CaptureScreen(ULONG quality);

    CaptureData *_curCapture;
    CaptureData *_prevCapture;

private:
    void AssignTargetDimensions(int targetWidth, int targetHeight);
    void AssignHeaders();
    int GetSizeDIB();
    void SwapCaptures();

    BITMAPFILEHEADER _bmpHeader;
    BITMAPINFO _bmpInfo;

    TargetDimensions _target;
    int _fullDIBSize;

    DesktopScreenData _desktopScreen;
    CaptureData _captureOne, _captureTwo;
};