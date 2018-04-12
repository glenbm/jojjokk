#pragma once

#include "atm.h"
#include <windows.h>

#include "GdiplusInit.h"

class Test {
public:

    struct CaptureDimensions {
        int width;
        int height;
        int x;
        int y;
    };

    HWND hwndCapture;
    RECT rectCapture;

    HDC hdcScreen, hMemDC;
    HBITMAP hBitmap, hOldBitmap;
    LPVOID pBits;
    BITMAPINFO bmi;

    std::vector<char> jpgBuffer{};

    int _width, _height;
    DesktopScreenData *_desktopData;
    CaptureDimensions captureDimensions;

    static const int BASE_ENCODER_QUALITY = 70;

    Test(DesktopScreenData *desktopData, int captureWidth, int captureHeight, int captureX, int captureY, int width, int height) {
        _desktopData = desktopData;
        captureDimensions.x = captureX;
        captureDimensions.y = captureY;
        captureDimensions.width = captureWidth;
        captureDimensions.height = captureHeight;

        _width = width;
        _height = height;
    }

    void GrabScreen() {
        hdcScreen = _desktopData->handle;
        hMemDC = CreateCompatibleDC(hdcScreen);

        bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = _width;
        bmi.bmiHeader.biHeight = -_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = _desktopData->bpp;

        hBitmap = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

        HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);

        BitBltOrStretchBlt();

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(0, hdcScreen);
    }

    void BitBltOrStretchBlt() {
        if (_width == _desktopData->width && _height == _desktopData->height) {
            BitBlt(hMemDC, 0, 0, _desktopData->width, _desktopData->height, hdcScreen, 0, 0, SRCCOPY);
            return;
        }

        SetStretchBltMode(hdcScreen, HALFTONE);
        StretchBlt(
            hMemDC,
            0,
            0,
            bmi.bmiHeader.biWidth,
            -bmi.bmiHeader.biHeight,
            hdcScreen,
            0,
            0,
            1920,
            1080,
            SRCCOPY
        );
    }

    void SaveToBuffer_JPG(ULONG quality) {
        SaveToBuffer(ImageType::JPG, quality);
    }

    void SaveToBuffer_BMP() {
        SaveToBuffer(ImageType::BMP);
    }

    void SaveToDisk_JPG(ULONG quality) {
        SaveToDisk(ImageType::JPG, quality);
    }

    void SaveToDisk_BMP() {
        SaveToDisk(ImageType::BMP);
    }


private:

    void SaveToBuffer(ImageType type, LONG quality = BASE_ENCODER_QUALITY) {
        GdiplusInit initGDI;
        CLSID clisd;

        IStream *stream = NULL;
        CreateStreamOnHGlobal(NULL, FALSE, (LPSTREAM*)&stream);

        Gdiplus::Bitmap *bmp = new  Gdiplus::Bitmap(hBitmap, (HPALETTE)NULL);

        GetEncoderClsid(type, &clisd);

        Gdiplus::EncoderParameters encoderParams;
        encoderParams.Count = 1;
        encoderParams.Parameter[0].NumberOfValues = 1;
        encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
        encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
        encoderParams.Parameter[0].Value = &quality;

        bmp->Save(stream, &clisd, &encoderParams);

        const LARGE_INTEGER largeIntegerBeginning = { 0 };
        ULARGE_INTEGER largeIntEnd = { 0 };
        stream->Seek(largeIntegerBeginning, STREAM_SEEK_CUR, &largeIntEnd);
        int jpegBufferSize = (ULONG)largeIntEnd.QuadPart;

        HGLOBAL pngInMemory;
        const HRESULT hResult = GetHGlobalFromStream(stream, &pngInMemory);

        LPVOID lpPngStreamBytes = GlobalLock(pngInMemory);

        char *charBuf = (char*)lpPngStreamBytes;
        jpgBuffer = std::vector<char>(charBuf, charBuf + jpegBufferSize);
    }

    void SaveToDisk(ImageType type, LONG quality = BASE_ENCODER_QUALITY) {
        GdiplusInit initGDI;
        CLSID clisd;

        Gdiplus::Bitmap bmp(hBitmap, NULL);

        GetEncoderClsid(type, &clisd);

        switch (type)
        {
        case ImageType::BMP:
            bmp.Save(L"C:\\Users\\e5522198\\Desktop\\BMP.bmp", &clisd, NULL);
            break;
        case ImageType::JPG:
            Gdiplus::EncoderParameters encoderParams;
            encoderParams.Count = 1;
            encoderParams.Parameter[0].NumberOfValues = 1;
            encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
            encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
            encoderParams.Parameter[0].Value = &quality;

            bmp.Save(L"C:\\Users\\e5522198\\Desktop\\JPG.jpeg", &clisd);
            break;
        default:

            break;
        }

    }

};

void GO() {
}