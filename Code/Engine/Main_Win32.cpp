#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Time.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "BitmapFont.hpp"
#include "EngineCommon.hpp"
#include "OpenGLRenderer.hpp"
#include "DeveloperConsole.hpp"
#include "../Game/World.hpp"
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" ) // Link in the GLU32.lib static library

//-----------------------------------------------------------------------------------------------
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;


//-----------------------------------------------------------------------------------------------
World g_world( (float) SCREEN_WIDTH, (float) SCREEN_HEIGHT );
unsigned char* g_pixelData;
OpenGLRenderer g_glRenderer;
Texture* g_diffuseTexture;
Texture* g_normalTexture;
Texture* g_specularTexture;
Texture* g_emissiveTexture;
Texture* g_depthTexture;
Texture* g_obamaTexture;
Texture* g_kfcTexture;
Texture* g_ladybugTexture;
Texture* g_mushroomTexture;
Texture* g_mainFontGlyphSheet;
DeveloperConsole g_developerConsole;


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "SD3";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	switch( wmMessageCode )
	{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			g_isQuitting = true;
			return 0;

		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;
			if( asKey == VK_ESCAPE && !g_developerConsole.m_drawConsole )
			{
				g_isQuitting = true;
				return 0;
			}

			bool wasProcessed = g_world.ProcessKeyDownEvent( asKey );
			if( wasProcessed )
			{
				return 0;
			}

			break;
		}
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			bool wasProcessed = g_world.ProcessKeyUpEvent( asKey );
			if( wasProcessed )
			{
				return 0;
			}

			break;
		}
		case WM_CHAR:
		{
			unsigned char asChar = (unsigned char) wParam;
			bool wasProcessed = g_world.ProcessCharDownEvent( asChar );
			if( wasProcessed )
			{
				return 0;
			}

			break;
		}
	}

	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle )
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );

	RECT windowRect = { 50 + 0, 50 + 0, 50 + SCREEN_WIDTH, 50 + SCREEN_HEIGHT };
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle)/sizeof(windowTitle[0]) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
	ShowCursor( FALSE );

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize			= sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion		= 1;
	pixelFormatDescriptor.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType	= PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits	= 24;
	pixelFormatDescriptor.cDepthBits	= 24;
	pixelFormatDescriptor.cAccumBits	= 0;
	pixelFormatDescriptor.cStencilBits	= 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );

	// Set up OpenGL states
	g_glRenderer.EnableBlend();
	g_glRenderer.EnableDepthTest();
	g_glRenderer.EnableCullFace();
	g_glRenderer.CullFrontFaceDirection( COUNTER_CLOCKWISE );
	g_glRenderer.BlendFunction( SRC_ALPHA, ONE_MINUS_SRC_ALPHA );
}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionClear( const ConsoleCommandArgs& )
{
	g_developerConsole.m_consoleLogLines.clear();
	return true;
}


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionQuit( const ConsoleCommandArgs& )
{
	g_isQuitting = true;
	return true;
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	g_world.Update();
}


//-----------------------------------------------------------------------------------------------
void Render()
{
	g_glRenderer.SetClearColor( 0.f, 0.2f, 0.4f, 1.f );
	g_glRenderer.SetClearDepth( 1.f );
	g_glRenderer.ClearColorBufferBit();
	g_glRenderer.ClearDepthBufferBit();

	g_world.Render();

	SwapBuffers( g_displayDeviceContext );
}


//-----------------------------------------------------------------------------------------------
void WaitUntilNextFrameTime()
{
	double timeNow = GetCurrentTimeSeconds();
	static double targetTime = timeNow;
	while( timeNow < targetTime )
	{
		timeNow = GetCurrentTimeSeconds();
	}
	targetTime = timeNow + FRAME_TIME_SECONDS;
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	RunMessagePump();
	Update();
	Render();
	WaitUntilNextFrameTime();
}


//-----------------------------------------------------------------------------------------------
void LoadTextures()
{
	g_diffuseTexture = Texture::CreateOrGetTexture( "Data/Images/cobblestonesDiffuse.png" );
	g_normalTexture = Texture::CreateOrGetTexture( "Data/Images/cobblestonesNormal.png" );
	g_specularTexture = Texture::CreateOrGetTexture( "Data/Images/cobblestonesSpecular.png" );
	g_emissiveTexture = Texture::CreateOrGetTexture( "Data/Images/cobblestonesEmissive.png" );
	g_depthTexture = Texture::CreateOrGetTexture( "Data/Images/cobblestonesDepth.png" );
	g_obamaTexture = Texture::CreateOrGetTexture( "Data/Images/Obama.png" );
	g_kfcTexture = Texture::CreateOrGetTexture( "Data/Images/KFC.png" );
	g_ladybugTexture = Texture::CreateOrGetTexture( "Data/Images/Ladybug.png" );
	g_mushroomTexture = Texture::CreateOrGetTexture( "Data/Images/Mushroom.png" );
	g_mainFontGlyphSheet = Texture::CreateOrGetTexture( "Data/Fonts/MainFont_EN_00.png" );
}


//-----------------------------------------------------------------------------------------------
void LoadDeveloperConsole()
{
	g_developerConsole = DeveloperConsole( Vector2( 0.f, (float) SCREEN_HEIGHT * 0.25f ), Vector2( (float) SCREEN_WIDTH, (float) SCREEN_HEIGHT ) );
	g_developerConsole.AddCommandFuncPtr( "clear", ConsoleFunctionClear );
	g_developerConsole.AddCommandFuncPtr( "quit", ConsoleFunctionQuit );
}


//-----------------------------------------------------------------------------------------------
void DeleteTexture( Texture* texture )
{
	if( texture->m_size.x >= 0 && texture->m_size.y >= 0 )
	{
		delete texture;
	}
}


//-----------------------------------------------------------------------------------------------
void UnloadTextures()
{
	DeleteTexture( g_diffuseTexture );
	DeleteTexture( g_normalTexture );
	DeleteTexture( g_specularTexture );
	DeleteTexture( g_emissiveTexture );
	DeleteTexture( g_depthTexture );
	DeleteTexture( g_obamaTexture );
	DeleteTexture( g_kfcTexture );
	DeleteTexture( g_ladybugTexture );
	DeleteTexture( g_mushroomTexture );
	DeleteTexture( g_mainFontGlyphSheet );
	Texture::DeconstructTexture();
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	CreateOpenGLWindow( applicationInstanceHandle );
	InitializeTime();
	LoadTextures();
	LoadDeveloperConsole();
	g_world.Initialize();
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );

	Initialize( applicationInstanceHandle );

	while( !g_isQuitting )	
	{
		RunFrame();
	}

	g_world.Deconstruct();
	UnloadTextures();

#if defined( _WIN32 ) && defined( _DEBUG )
	assert( _CrtCheckMemory() );
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}