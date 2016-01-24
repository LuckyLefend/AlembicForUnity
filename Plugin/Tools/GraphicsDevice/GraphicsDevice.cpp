#include "pch.h"
#include "Common.h"
#include "GraphicsDevice.h"


int tGetPixelSize(tTextureFormat format)
{
    switch (format)
    {
    case tTextureFormat_ARGB32:    return 4;

    case tTextureFormat_ARGBHalf:  return 8;
    case tTextureFormat_RGHalf:    return 4;
    case tTextureFormat_RHalf:     return 2;

    case tTextureFormat_ARGBFloat: return 16;
    case tTextureFormat_RGFloat:   return 8;
    case tTextureFormat_RFloat:    return 4;

    case tTextureFormat_ARGBInt:   return 16;
    case tTextureFormat_RGInt:     return 8;
    case tTextureFormat_RInt:      return 4;
    }
    return 0;
}


namespace {
    thread_local std::vector<char> *g_conversion_buffer;
}

void* tGetConversionBuffer(size_t size)
{
    if (g_conversion_buffer == nullptr)
    {
        g_conversion_buffer = new std::vector<char>();
    }
    g_conversion_buffer->resize(size);
    return &(*g_conversion_buffer)[0];
}



tIGraphicsDevice* aiCreateGraphicsDeviceOpenGL(void *device);
tIGraphicsDevice* aiCreateGraphicsDeviceD3D9(void *device);
tIGraphicsDevice* aiCreateGraphicsDeviceD3D11(void *device);


tIGraphicsDevice *g_theGraphicsDevice;
tCLinkage tExport tIGraphicsDevice* tGetGraphicsDevice() { return g_theGraphicsDevice; }
typedef tIGraphicsDevice* (*aiGetGraphicsDeviceT)();


tCLinkage tExport void UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
{
    if (eventType == kGfxDeviceEventInitialize) {
#ifdef tSupportD3D9
        if (deviceType == kGfxRendererD3D9)
        {
            g_theGraphicsDevice = aiCreateGraphicsDeviceD3D9(device);
        }
#endif // tSupportD3D9
#ifdef tSupportD3D11
        if (deviceType == kGfxRendererD3D11)
        {
            g_theGraphicsDevice = aiCreateGraphicsDeviceD3D11(device);
        }
#endif // tSupportD3D11
#ifdef tSupportOpenGL
        if (deviceType == kGfxRendererOpenGL)
        {
            g_theGraphicsDevice = aiCreateGraphicsDeviceOpenGL(device);
        }
#endif // tSupportOpenGL
    }

    if (eventType == kGfxDeviceEventShutdown) {
        delete g_theGraphicsDevice;
        g_theGraphicsDevice = nullptr;
    }
}

tCLinkage tExport void UnityRenderEvent(int eventID)
{
}


#ifdef tSupportOpenGL
tCLinkage tExport void tInitializeOpenGL()
{
    UnitySetGraphicsDevice(nullptr, kGfxRendererOpenGL, kGfxDeviceEventInitialize);
}
#endif

#ifdef tSupportD3D9
tCLinkage tExport void tInitializeD3D9(void *device)
{
    UnitySetGraphicsDevice(device, kGfxRendererD3D9, kGfxDeviceEventInitialize);
}
#endif

#ifdef tSupportD3D11
tCLinkage tExport void tInitializeD3D11(void *device)
{
    UnitySetGraphicsDevice(device, kGfxRendererD3D11, kGfxDeviceEventInitialize);
}
#endif

tCLinkage tExport void tFinalizeGraphicsDevice()
{
    UnitySetGraphicsDevice(nullptr, kGfxRendererNull, kGfxDeviceEventShutdown);
}