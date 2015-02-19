#ifndef include_OpenGLRenderer
#define include_OpenGLRenderer
#pragma once

//-----------------------------------------------------------------------------------------------
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <vector>
#include "Renderer.hpp"
#include "glext.h"
#include "Color.hpp"
#include "Vertex.hpp"
#include "Vector2.hpp"
#include "BitmapFont.hpp"
#include "MatrixStack44.hpp"


//-----------------------------------------------------------------------------------------------
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;


//-----------------------------------------------------------------------------------------------
class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer();
	void PushMatrix();
	void PopMatrix();
	void EnableTexture2D();
	void DisableTexture2D();
	void SetActiveTexture( textureUnit texUnit );
	void GenerateTextures( unsigned int* textures );
	void SetTexture2DImage( int level, pixelDataFormat internalFormat, int width, int height, int border, pixelDataFormat bufferFormat, const void* pixels );
	void BindTexture2D( unsigned int textureID );
	void GenerateMipmapTexture2D();
	void GenerateMipmapHint();
	void PixelStore();
	void Translatef( float translateX, float translateY, float translateZ );
	void Rotatef( float angleDegree, float rotateX, float rotateY, float rotateZ );
	void Scalef( float scaleX, float scaleY, float scaleZ );
	void SetColor3f( float red, float green, float blue );
	void SetColor4f( float red, float green, float blue, float alpha );
	void SetLineWidth( float lineWidth );
	void SetDepthMask( bool depthMaskOnOrOff );
	void SetOrtho( float leftCoord, float rightCoord, float bottomCoord, float topCoord, float zNearDistance, float zFarDistance );
	void SetPerspective( float fovyDegrees, float aspectRatio, float zNearDistance, float zFarDistance );
	void BeginRender( renderType shape );
	void EndRender();
	void SetVertex2f( float vertX, float vertY );
	void SetVertex3f( float vertX, float vertY, float vertZ );
	void SetTexCoords2f( float coordX, float coordY );
	void SetVBO( unsigned int* VBOid, const std::vector<Vertex>& vertices );
	void RenderVertices( const std::vector<Vertex>& vertices, renderType shape );
	void CreateColorFramebufferTexture2D( unsigned int* framebufferColorTexID, textureUnit texUnit, int textureWidth, int textureHeight );
	void CreateDepthFramebufferTexture2D( unsigned int* framebufferDepthTexID, textureUnit texUnit, int textureWidth, int textureHeight );
	void SetFBO( unsigned int* FBOid, unsigned int framebufferColorTexID, unsigned int framebufferDepthTexID );
	void BindFramebuffer( unsigned int FBOid );
	void DisableFramebuffer();
	void MultMatrixf( const float* mat );
	void GenerateBuffers( int size, unsigned int* buffers );
	void DeleteBuffers( int size, unsigned int* buffers );
	void BindBuffer( unsigned int buffer );
	void SetBufferData( int size, const void* data );
	void EnableClientState( arrayType aType );
	void DisableClientState( arrayType aType );
	void DrawArrays( renderType shape, int first, unsigned int count );
	void EnableCullFace();
	void CullFrontFaceDirection( frontFaceDirection dirForFontFace );
	void CullFaceSide( faceSide frontOrBack );
	void ClearColorBufferBit();
	void ClearDepthBufferBit();
	void SetClearColor( float red, float green, float blue, float alpha );
	void SetClearDepth( float depth );
	void EnableBlend();
	void EnableDepthTest();
	void DisableDepthTest();
	void BlendFunction(  blendFuncParam sFactor, blendFuncParam dFactor  );
	void SetTexture2DWrapS( textureWrap clampOrRepeat );
	void SetTexture2DWrapT( textureWrap clampOrRepeat );
	void SetTexture2DMagnificationFilter( textureFilter filter );
	void SetTexture2DMinificationFilter( textureFilter filter );
	void SetTexture2DMaxLevel( int param );
	void UseShaderProgram( unsigned int shaderProgramID );
	int GetUniformLocation( unsigned int shaderProgramID, const char* name );
	void SetUniform1i( int uniformLocation, int val );
	void SetUniform1f( int uniformLocation, float val );
	void SetUniform2f( int uniformLocation, float val0, float val1 );
	void SetUniform3f( int uniformLocation, float val0, float val1, float val2 );
	void SetUniform4f( int uniformLocation, float val0, float val1, float val2, float val3 );
	void SetModelViewProjectionMatrix( int uniformLocation );
	void SetPositionAttribPointer( int attribLocation );
	void SetColorAttribPointer( int attribLocation );
	void SetTexCoordAttribPointer( int attribLocation );
	void SetNormalAttribPointer( int attribLocation );
	void SetTangentAttribPointer( int attribLocation );
	void EnableAttribArray( int attribLocation );
	void DisableAttribArray( int attribLocation );
	void RenderText( const std::string& text, const BitmapFont& font, float fontCellHeight, const Vector2& screenPos );
	void RenderText( const std::string& text, const BitmapFont& font, float fontCellHeight, const Vector2& screenPos, const Color& color );
	void SetTextVertices( std::vector<Vertex>& vertices, const std::string& text, const BitmapFont& font, float fontCellHeight, const Vector2& screenPos );
	void SetTextVertices( std::vector<Vertex>& vertices, const std::string& text, const BitmapFont& font, float fontCellHeight, const Vector2& screenPos, const Color& color );
	float CalcTextWidth( const std::string& text, const BitmapFont& font, float fontCellHeight );

private:
	int GetBlendParameter( blendFuncParam param );
};


#endif // include_OpenGLRenderer