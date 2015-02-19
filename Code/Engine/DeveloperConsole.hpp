#ifndef include_DeveloperConsole
#define include_DeveloperConsole
#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include <vector>
#include "Color.hpp"
#include "Vertex.hpp"
#include "Vector2.hpp"
#include "Keyboard.hpp"
#include "Material.hpp"
#include "BitmapFont.hpp"
#include "ConsoleLogLine.hpp"
#include "OpenGLRenderer.hpp"
#include "ConsoleCommandArgs.hpp"
#include "OpenGLShaderProgram.hpp"


//-----------------------------------------------------------------------------------------------
typedef bool (*commandFuncPtr)(const ConsoleCommandArgs& args);


//-----------------------------------------------------------------------------------------------
const int CHAR_VALUES_START = 32;
const float CURSOR_BLINKS_PER_SECOND = 1.5f;
const float SECONDS_FOR_CURSOR_BLINK_TOGGLE = 1.f / ( 2.f * CURSOR_BLINKS_PER_SECOND );
const float CURSOR_LINE_WIDTH = 1.f;
const float FONT_CELL_HEIGHT_PIXELS = 30.f;
const float COMMAND_PROMPT_HEIGHT_PIXELS = FONT_CELL_HEIGHT_PIXELS * 1.2f;
const float COMMAND_PROMPT_OUTLINE_LINE_WIDTH = 3.f;
const float COMMAND_PROMPT_TEXT_SPACING = 7.f;
const float FONT_SHADOW_OFFSET_PIXELS_X = 2.f;
const float FONT_SHADOW_OFFSET_PIXELS_Y = -2.f;
const Color CONSOLE_BACKGROUND_COLOR = Color( 0.1f, 0.1f, 0.1f, 0.7f );
const Color COMMAND_PROMPT_OUTLINE_COLOR = Color( 1.f, 1.f, 1.f, 1.f );
const Color FUNCTION_NOT_FOUND_LINE_COLOR = Color( 1.f, 0.f, 0.f, 1.f );
const Color FUNCTION_UNSUCCESSFUL_LINE_COLOR = Color( 1.f, 1.f, 0.f, 1.f );
const Color FUNCTION_SUCCESS_LINE_COLOR = Color( 1.f, 1.f, 1.f, 1.f );
const Color FONT_SHADOW_COLOR = Color( 0.f, 0.f, 0.f, 1.f );
const std::string MAIN_FONT_GLYPH_SHEET_FILE_NAME = "Data/Fonts/MainFont_EN_00.png";
const std::string MAIN_FONT_META_DATA_FILE_NAME = "Data/Fonts/MainFont_EN.FontDef.xml";
const std::string CONSOLE_VERTEX_SHADER_STRING = "Data/Shaders/Simple_No_Texture_330.vertex.glsl";
const std::string CONSOLE_FRAGMENT_SHADER_STRING = "Data/Shaders/Simple_No_Texture_330.fragment.glsl";
const std::string TEXT_VERTEX_SHADER_STRING = "Data/Shaders/Simple_Texture_330.vertex.glsl";
const std::string TEXT_FRAGMENT_SHADER_STRING = "Data/Shaders/Simple_Texture_330.fragment.glsl";


//-----------------------------------------------------------------------------------------------
class DeveloperConsole
{
public:
	DeveloperConsole() {}
	DeveloperConsole( const Vector2& bottomLeftPosition, const Vector2& topRightPosition );
	void AddCommandFuncPtr( const std::string& functionName, commandFuncPtr functionPtr );
	void Update( const Keyboard& keyboard, float deltaSeconds );
	void Render();

	bool							m_drawConsole;
	std::vector<ConsoleLogLine>		m_consoleLogLines;

private:
	void SetConsoleVertices();
	void SetTextVertices();
	void CheckForKeyPress( const Keyboard& keyboard );
	void CheckForCharDelete( const Keyboard& keyboard );
	void CheckForCursorMovement( const Keyboard& keyboard );
	void CheckForLogHistoryMovement( const Keyboard& keyboard );
	void CheckForCommandExecute( const Keyboard& keyboard );
	void ExecuteConsoleCommand( const std::string& consoleCommandName, const std::string& argsString );

	Vector2									m_bottomLeftPosition;
	Vector2									m_topRightPosition;
	BitmapFont								m_font;
	unsigned int							m_cursorPosition;
	unsigned int							m_consoleTextEntryPosition;
	unsigned int							m_consoleBackgroundVBOid;
	unsigned int							m_commandPromptVBOid;
	unsigned int							m_cursorVBOid;
	unsigned int							m_consoleTextVBOid;
	bool									m_drawCursor;
	float									m_secondsSinceCursorRenderToggle;
	Material*								m_consoleMaterial;
	Material*								m_textMaterial;
	OpenGLRenderer							m_glRenderer;
	ConsoleLogLine							m_commandPromptLine;
	std::vector<Vertex>						m_consoleBackgroundVertices;
	std::vector<Vertex>						m_commandPromptVertices;
	std::vector<Vertex>						m_cursorVertices;
	std::vector<Vertex>						m_consoleTextVertices;
	std::vector<ConsoleLogLine>				m_consoleLogHistoryLines;
	std::map<std::string, commandFuncPtr>	m_commandFunctionPtrs;
};


#endif // include_DeveloperConsole