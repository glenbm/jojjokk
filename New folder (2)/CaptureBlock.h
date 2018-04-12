#pragma once

#include "atm.h"
#include <windows.h>
#include <vector>


class CaptureBlock {

public:

    CaptureBlock() {
    
    };

    CaptureBlock(HDC screen, int _height, int _width, int captureWidth, int captureHeight, int captureX, int captureY) {
        GetBmpInfo();

        _screenHandle = CreateCompatibleDC(screen);
        _pixelBuffer = NULL;
        _hBitmap = CreateDIBSection(screen, &_bmpInfo, DIB_RGB_COLORS, &_pixelBuffer, NULL, 0);

        SelectObject(_screenHandle, _hBitmap);

        PatBlt(_screenHandle, 0, 0, _width, _height, BLACKNESS);

        _stride = ((((_width * _bitsPerPixel) + 31) & ~31) >> 3);
        _bufferSize = _stride * _height;
    }

    void GrabDesktop(HDC screen) {
        SetStretchBltMode(_screenHandle, HALFTONE);
        StretchBlt(
            _screenHandle,
            0,
            0,
            _width,
            _height,
            screen,
            _captureX,
            _captureY,
            _captureWidth,
            _captureHeight,
            SRCCOPY
        );
    }

    void EncodeJpeg(ULONG quality) {
        GdiplusInit gdiplusInit;

        CLSID encoderClsid;
        int result = GetEncoderClsid(ImageType::JPG, &encoderClsid);

        IStream *stream = NULL;
        CreateStreamOnHGlobal(NULL, FALSE, (LPSTREAM*)&stream);

        Gdiplus::EncoderParameters encoderParams;
        encoderParams.Count = 1;
        encoderParams.Parameter[0].NumberOfValues = 1;
        encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
        encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
        encoderParams.Parameter[0].Value = &quality;

        Gdiplus::Bitmap *bmp = new  Gdiplus::Bitmap(_hBitmap, (HPALETTE)NULL);

        bmp->Save(stream, &encoderClsid, &encoderParams);
        //bmp->Save(L"C:\\Users\\e5522198\\Desktop\\CUR.jpg", &encoderClsid, &encoderParams);

        const LARGE_INTEGER largeIntegerBeginning = { 0 };
        ULARGE_INTEGER largeIntEnd = { 0 };
        stream->Seek(largeIntegerBeginning, STREAM_SEEK_CUR, &largeIntEnd);
        _jpgBufferSize = (ULONG)largeIntEnd.QuadPart;

        HGLOBAL pngInMemory;
        const HRESULT hResult = GetHGlobalFromStream(stream, &pngInMemory);

        LPVOID lpPngStreamBytes = GlobalLock(pngInMemory);

        char *charBuf = (char*)lpPngStreamBytes;
        _jpgBuffer = std::vector<char>(charBuf, charBuf + _jpgBufferSize);
    }

private:
    HDC _screenHandle;
    void* _pixelBuffer;
    HBITMAP _hBitmap;
    BITMAPINFO _bmpInfo;

    int _bitsPerPixel;
    int _bufferSize;
    int _jpgBufferSize;

    int _width, _height;

    int _stride;

    int _captureHeight, _captureWidth;
    int _captureX, _captureY;

    std::vector<char> _jpgBuffer = {};

    void GetBmpInfo() {
        _bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        _bmpInfo.bmiHeader.biWidth = _width;
        _bmpInfo.bmiHeader.biHeight = -_height;
        _bmpInfo.bmiHeader.biPlanes = 1;
        _bmpInfo.bmiHeader.biBitCount = _bitsPerPixel;
        _bmpInfo.bmiHeader.biCompression = BI_RGB;
        _bmpInfo.bmiHeader.biSizeImage = 0;
        _bmpInfo.bmiHeader.biXPelsPerMeter = 0;
        _bmpInfo.bmiHeader.biYPelsPerMeter = 0;
        _bmpInfo.bmiHeader.biClrUsed = 0;
        _bmpInfo.bmiHeader.biClrImportant = 0;
    }

};