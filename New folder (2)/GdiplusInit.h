#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <stdexcept>
using std::runtime_error;

#define GDIPVER 0x0110
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

struct GdiplusInit {
    GdiplusInit() {
        Gdiplus::GdiplusStartupInput inp;
        Gdiplus::GdiplusStartupOutput outp;
        if (Gdiplus::Ok != GdiplusStartup(&token_, &inp, &outp))
            throw runtime_error("GdiplusStartup");
    }
    ~GdiplusInit() {
        Gdiplus::GdiplusShutdown(token_);
    }
private:
    ULONG_PTR token_;
};

enum ImageType {
    BMP,
    JPG,
    GIF,
    TIFF,
    PNG
};

inline int GetEncoderClsid(ImageType imgage_format, CLSID* pClsid)
{
    WCHAR* format = NULL;

    switch (imgage_format)
    {
    case ImageType::BMP:
            format = L"image/bmp";
            break;
    case ImageType::JPG:
            format = L"image/jpeg";
            break;
    default:
        break;
    }

    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}