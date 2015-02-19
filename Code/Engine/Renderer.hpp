#ifndef include_Renderer
#define include_Renderer
#pragma once

//-----------------------------------------------------------------------------------------------
#include "MatrixStack44.hpp"


//-----------------------------------------------------------------------------------------------
enum renderType {
	LINES,
	TRIANGLES,
	QUADS,
};


//-----------------------------------------------------------------------------------------------
enum arrayType {
	VERTEX_ARRAY,
	COLOR_ARRAY,
	TEXTURE_COORD_ARRAY,
	NORMAL_ARRAY,
};


//-----------------------------------------------------------------------------------------------
enum frontFaceDirection {
	CLOCKWISE,
	COUNTER_CLOCKWISE,
};


//-----------------------------------------------------------------------------------------------
enum faceSide {
	FRONT,
	BACK,
};


//-----------------------------------------------------------------------------------------------
enum textureWrap {
	CLAMP,
	REPEAT,
};


//-----------------------------------------------------------------------------------------------
enum textureFilter {
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_NEAREST,
	LINEAR_MIPMAP_LINEAR,
};


//-----------------------------------------------------------------------------------------------
enum pixelDataFormat {
	RGB,
	RGBA,
};


//-----------------------------------------------------------------------------------------------
enum blendFuncParam {
	ZERO,
	ONE,
	DST_COLOR,
	ONE_MINUS_DST_COLOR,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
};


//-----------------------------------------------------------------------------------------------
enum textureUnit {
	TEXTURE0,
	TEXTURE1,
	TEXTURE2,
	TEXTURE3,
	TEXTURE4,
	TEXTURE5,
	TEXTURE6,
	TEXTURE7,
};


//-----------------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer() {}
	virtual void PushMatrix() {}
	virtual void PopMatrix() {}
	virtual void EnableTexture2D() {}
	virtual void DisableTexture2D() {}
	virtual void SetActiveTexture() {}
	virtual void GenerateTextures() {}
	virtual void SetTexture2DImage() {}
	virtual void BindTexture2D() {}
	virtual void GenerateMipmapTexture2D() {}
	virtual void GenerateMipmapHint() {}
	virtual void PixelStore() {}
	virtual void Translatef() {}
	virtual void Rotatef() {}
	virtual void Scalef() {}
	virtual void SetColor3f() {}
	virtual void SetColor4f() {}
	virtual void SetLineWidth() {}
	virtual void SetDepthMask() {}
	virtual void SetOrtho () {}
	virtual void SetPerspective() {}
	virtual void BeginRender() {}
	virtual void EndRender() {}
	virtual void SetVertex2f() {}
	virtual void SetVertex3f() {}
	virtual void SetTexCoords2f() {}
	virtual void SetVBO() {}
	virtual void RenderVertices() {}
	virtual void CreateColorFramebufferTexture2D() {}
	virtual void CreateDepthFramebufferTexture2D() {}
	virtual void SetFBO() {}
	virtual void BindFramebuffer() {}
	virtual void DisableFramebuffer() {}
	virtual void GenerateBuffers() {}
	virtual void DeleteBuffers() {}
	virtual void BindBuffer() {}
	virtual void SetBufferData() {}
	virtual void EnableClientState() {}
	virtual void DisableClientState() {}
	virtual void DrawArrays() {}
	virtual void EnableCullFace() {}
	virtual void CullFrontFaceDirection() {}
	virtual void CullFaceSide() {}
	virtual void ClearColorBufferBit() {}
	virtual void ClearDepthBufferBit() {}
	virtual void SetClearColor() {}
	virtual void SetClearDepth() {}
	virtual void EnableBlend() {}
	virtual void EnableDepthTest() {}
	virtual void BlendFunction() {}
	virtual void SetTexture2DWrapS() {}
	virtual void SetTexture2DWrapT() {}
	virtual void SetTexture2DMagnificationFilter() {}
	virtual void SetTexture2DMinificationFilter() {}
	virtual void SetTexture2DMaxLevel() {}
	virtual void UseShaderProgram() {}
	virtual int GetUniformLocation() { return 0; }
	virtual void SetUniform1i() {}
	virtual void SetUniform1f() {}
	virtual void SetUniform2f() {}
	virtual void SetUniform3f() {}
	virtual void SetUniform4f() {}
	virtual void SetModelViewProjectionMatrix() {}
	virtual void SetPositionAttribPointer() {}
	virtual void SetColorAttribPointer() {}
	virtual void SetTexCoordAttribPointer() {}
	virtual void SetNormalAttribPointer() {}
	virtual void SetTangentAttribPointer() {}
	virtual void EnableAttribArray() {}
	virtual void DisableAttribArray() {}
	virtual void RenderText() {}
	virtual float CalcTextWidth() { return 0.f; }

protected:
	static MatrixStack44	s_matrixStack;
};


#endif // include_Renderer