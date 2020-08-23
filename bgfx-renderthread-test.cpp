#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>

//TODO: could support GLFW_EXPOSE_NATIVE_EGL
#if BX_PLATFORM_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif
#include "GLFW/glfw3native.h"


int main(void)
{
    bgfx::RendererType::Enum preferredRenderer = bgfx::RendererType::OpenGL;
    int width = 640, height = 480;

    bgfx::RendererType::Enum supported_renderers[bgfx::RendererType::Count];
    const uint32_t supported_renderer_count = bgfx::getSupportedRenderers( bgfx::RendererType::Count, supported_renderers );
    bool preferredRendererSupported = false;
    for (uint32_t i = 0; i < supported_renderer_count; i++)
    {
        if( supported_renderers[i] == preferredRenderer )
        {
            preferredRendererSupported = true;
            break;
        }
    }
    if( !preferredRendererSupported )
    {
        preferredRenderer = bgfx::RendererType::Noop;
    }

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    bgfx::PlatformData pd;
    GLFWwindow* window = nullptr;

    if( preferredRenderer != bgfx::RendererType::OpenGL )
    {
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    }

    window = glfwCreateWindow(width, height, "bgfx-renderthread-test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

#if BX_PLATFORM_LINUX
    if( preferredRenderer == bgfx::RendererType::OpenGL )
    {
        pd.context = glfwGetGLXContext(window);
    }
    pd.nwh = (void*)glfwGetGLXWindow(window);
#elif BX_PLATFORM_WINDOWS
    if( preferredRenderer == bgfx::RendererType::OpenGL )
    {
        pd.context = glfwGetWGLContext(window);
    }
    pd.nwh = glfwGetWin32Window(window); // do not need glfwGetWGLContext(window);
#elif BX_PLATFORM_OSX
    if( preferredRenderer == bgfx::RendererType::OpenGL )
    {
        pd.context = glfwGetNSGLContext(window);
    }
    else
    {
        pd.nwh = [glfwGetCocoaWindow(window) contentView];
    }
#endif

    bgfx::setPlatformData(pd);

    const bgfx::ViewId view_id = 0;
    
    glfwGetWindowSize(window, &width, &height);
    
    std::atomic<int> shutdown(0);

    std::thread draw_thread( [=, &shutdown, &width, &height]
    {
        bgfx::Init bgfx_init;
        bgfx_init.resolution.width = (uint32_t)width;
        bgfx_init.resolution.height = (uint32_t)height;
        bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
        bgfx_init.type = preferredRenderer;

        if (!bgfx::init(bgfx_init))
        {
            return;
        }
            
        bgfx::setViewClear(view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x7F003FFF, 1.0f, 0);
        bgfx::setViewRect(view_id, 0, 0, bgfx::BackbufferRatio::Equal);
            
        int prevWidth = width;
        int prevHeight = height;
        while(!shutdown) {
                
            bgfx::touch(view_id);
                
            // width and height may change outside loop, so we store in a temp variable
            // this will eventually lead to correct results, but in future should add better approach
            int newWidth  = width;
            int newHeight = height;
            if( newWidth != prevWidth || newHeight != prevHeight )
            {
                bgfx::setViewRect(view_id, 0, 0, newWidth, newHeight );
                bgfx::reset((uint32_t)newWidth, (uint32_t)newHeight, BGFX_RESET_VSYNC);
                prevWidth  = newWidth;
                prevHeight = newHeight;
            }
                
            const auto renderer_type = bgfx::getRendererType();
                
            bgfx::setDebug(BGFX_DEBUG_TEXT);
            bgfx::dbgTextClear();
            bgfx::dbgTextPrintf(0, 0, 0x0f, "Supported renderers:");
            for (uint32_t i = 0; i < supported_renderer_count; i++)
            {
                bgfx::dbgTextPrintf(0, i+1, 0x0f, "%s", bgfx::getRendererName(supported_renderers[i]));
            }
            bgfx::dbgTextPrintf(0, supported_renderer_count + 2, 0x0f, "Selected renderer: %s", bgfx::getRendererName(renderer_type));
            bgfx::dbgTextPrintf(0, supported_renderer_count + 3, 0x0f, "Width %d, Height %d", width, height );
                
            bgfx::frame();
        }
        bgfx::shutdown();

    });

    
    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents(); // in this example we use wait to ensure we do not loop needlessly
        glfwGetWindowSize(window, &width, &height);
    }
    
    shutdown = 1;

    draw_thread.join();
    glfwTerminate();
    
    return 0;
}
