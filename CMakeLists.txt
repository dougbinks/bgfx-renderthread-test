cmake_minimum_required( VERSION 3.1 )

project( bgfx-renderthread-test )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory( glfw )

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

if( MSVC )
	include_directories( bx/include/compat/msvc )
endif()
if( MINGW )
	include_directories( bx/include/compat/mingw )
endif()
if( APPLE )
	include_directories( bx/include/compat/osx )
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	include_directories( bx/include/compat/freebsd )
endif()
include_directories( bx/include bx/3rdparty )	
# for simplicity not adding headers to source
set( BX_SRC
	 bx/src/allocator.cpp
	 bx/src/bx_p.h
	 bx/src/file.cpp
	 bx/src/math.cpp
	 bx/src/process.cpp
	 bx/src/sort.cpp
	 bx/src/timer.cpp
	 bx/src/commandline.cpp
	 bx/src/dtoa.cpp
	 bx/src/filepath.cpp
	 bx/src/mutex.cpp
	 bx/src/semaphore.cpp
	 bx/src/string.cpp
	 bx/src/url.cpp
	 bx/src/bx.cpp
	 bx/src/easing.cpp
	 bx/src/hash.cpp
	 bx/src/os.cpp
	 bx/src/settings.cpp
	 bx/src/thread.cpp
	)

include_directories( bimg/include bimg/3rdparty bimg/3rdparty/astc-codec bimg/3rdparty/astc-codec/include bimg/3rdparty/iqa/include )	
set( BIMG_SRC
	 bimg/src/bimg_p.h
	 bimg/src/config.h
	 bimg/src/image.cpp
	 bimg/src/image_cubemap_filter.cpp
	 bimg/src/image_decode.cpp
	 bimg/src/image_encode.cpp
	 bimg/src/image_gnf.cpp
	 bimg/3rdparty/astc-codec/src/decoder/astc_file.cc
	 bimg/3rdparty/astc-codec/src/decoder/endpoint_codec.cc
	 bimg/3rdparty/astc-codec/src/decoder/integer_sequence_codec.cc
	 bimg/3rdparty/astc-codec/src/decoder/logical_astc_block.cc
	 bimg/3rdparty/astc-codec/src/decoder/physical_astc_block.cc
	 bimg/3rdparty/astc-codec/src/decoder/weight_infill.cc
	 bimg/3rdparty/astc-codec/src/decoder/codec.cc
	 bimg/3rdparty/astc-codec/src/decoder/footprint.cc
	 bimg/3rdparty/astc-codec/src/decoder/intermediate_astc_block.cc
	 bimg/3rdparty/astc-codec/src/decoder/partition.cc
	 bimg/3rdparty/astc-codec/src/decoder/quantization.cc
	)
	
include_directories( bgfx/include bgfx/3rdparty bgfx/3rdparty/dxsdk/include bgfx/3rdparty/khronos )
set( BGFX_SRC
	 bgfx/src/glcontext_egl.cpp
	 bgfx/src/nvapi.cpp
	 bgfx/src/renderer_gl.cpp
	 bgfx/src/renderer_vk.cpp
	 bgfx/src/shader_dxbc.cpp
	 bgfx/src/bgfx.cpp
	 bgfx/src/glcontext_glx.cpp
	 bgfx/src/renderer_d3d11.cpp
	 bgfx/src/renderer_gnm.cpp
	 bgfx/src/renderer_webgpu.cpp
	 bgfx/src/shader_spirv.cpp
	 bgfx/src/debug_renderdoc.cpp
	 bgfx/src/glcontext_html5.cpp
	 bgfx/src/renderer_d3d12.cpp
	 bgfx/src/renderer_noop.cpp
	 bgfx/src/shader.cpp
	 bgfx/src/topology.cpp
	 bgfx/src/dxgi.cpp
	 bgfx/src/glcontext_wgl.cpp
	 bgfx/src/renderer_d3d9.cpp
	 bgfx/src/shader_dx9bc.cpp
	 bgfx/src/vertexlayout.cpp
	)

if( APPLE )
list( APPEND BGFX_SRC
	  bgfx/src/amalgamated.mm
	  bgfx/src/glcontext_eagl.mm
	  bgfx/src/glcontext_nsgl.mm
	  bgfx/src/renderer_mtl.mm
	)
endif()
	
add_library( bx STATIC ${BX_SRC} )
add_library( bimg STATIC ${BIMG_SRC} )
add_library( bgfx STATIC ${BGFX_SRC} )

set( BGFX-RENDERTHREAD-TEST-SRC
     bgfx-renderthread-test.cpp
     )
	 
if( UNIX )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
endif()
if( APPLE )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
endif()
if( MSVC )
	add_definitions("/D\"_CRT_SECURE_NO_WARNINGS\"")
endif()
     
add_executable( bgfx-renderthread-test WIN32 ${BGFX-RENDERTHREAD-TEST-SRC} )

find_package( OpenGL )
if( OpenGL_FOUND )
	include_directories( ${OPENGL_INCLUDE_DIRS} )
	target_link_libraries( bgfx-renderthread-test ${OPENGL_LIBRARIES} bx bimg bgfx glfw )
else()
	target_link_libraries( bgfx-renderthread-test bx bimg bgfx glfw )
endif()

if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT bgfx-renderthread-test )
    endif()
endif()