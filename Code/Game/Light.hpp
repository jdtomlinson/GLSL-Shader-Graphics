#ifndef include_Light
#define include_Light
#pragma once

//-----------------------------------------------------------------------------------------------
#include <sstream>
#include "../Engine/Color.hpp"
#include "../Engine/Vector3.hpp"
#include "../Engine/Vector4.hpp"
#include "../Engine/Material.hpp"
#include "../Engine/DebugGraphics.hpp"


//-----------------------------------------------------------------------------------------------
class Light
{
public:
	Light() {}
	Light( const Vector3& position, const Vector3& direction, const Color& colorAndBrightness, float innerRadius, float outerRadius, float innerApertureDot, float outerApertureDot, float fractionAmbient, bool isPositionless );
	void UpdatePosition( const Vector3& position );
	void SetUniformValues( Material* material, int lightIndex );
	void Render();

	Vector3				m_position;
	Vector3				m_direction;
	Color				m_colorAndBrightnessAlpha;
	float				m_innerRadius;
	float				m_outerRadius;
	float				m_innerApertureDot;
	float				m_outerApertureDot;
	float				m_fractionAmbient;
	bool				m_isPositionless;
	DebugGraphicsPoint	m_lightPoint;
};


//-----------------------------------------------------------------------------------------------
inline Light::Light( const Vector3& position, const Vector3& direction, const Color& colorAndBrightness, float innerRadius, float outerRadius, float innerApertureDot, float outerApertureDot, float fractionAmbient, bool isPositionless )
	: m_position( position )
	, m_direction( direction )
	, m_colorAndBrightnessAlpha( colorAndBrightness )
	, m_innerRadius( innerRadius )
	, m_outerRadius( outerRadius )
	, m_innerApertureDot( innerApertureDot )
	, m_outerApertureDot( outerApertureDot )
	, m_fractionAmbient( fractionAmbient )
	, m_isPositionless( isPositionless )
{
	m_lightPoint = DebugGraphicsPoint( Vector3( position.x, position.y, position.z ), Color( colorAndBrightness.r, colorAndBrightness.g, colorAndBrightness.b ) );
}


//-----------------------------------------------------------------------------------------------
inline void Light::UpdatePosition( const Vector3& position )
{
	m_position.x = position.x;
	m_position.y = position.y;
	m_position.z = position.z;
	m_lightPoint.UpdatePosition( position );
}


//-----------------------------------------------------------------------------------------------
inline void Light::SetUniformValues( Material* material, int lightIndex )
{
	std::stringstream ss;
	ss << lightIndex;
	std::string uniformLightIndex = "u_lights[" + ss.str() + "].";

	material->SetUniform( uniformLightIndex + "m_position", m_position );
	material->SetUniform( uniformLightIndex + "m_colorAndBrightnessAlpha", m_colorAndBrightnessAlpha );
	material->SetUniform( uniformLightIndex + "m_innerRadius", m_innerRadius );
	material->SetUniform( uniformLightIndex + "m_outerRadius", m_outerRadius );
	material->SetUniform( uniformLightIndex + "m_direction", m_direction );
	material->SetUniform( uniformLightIndex + "m_innerApertureDot", m_innerApertureDot );
	material->SetUniform( uniformLightIndex + "m_outerApertureDot", m_outerApertureDot );
	material->SetUniform( uniformLightIndex + "m_fractionAmbient", m_fractionAmbient );
	material->SetUniform( uniformLightIndex + "m_isPositionless", m_isPositionless );
}


//-----------------------------------------------------------------------------------------------
inline void Light::Render()
{
	if( m_isPositionless )
		return;

	m_lightPoint.Render();
}


#endif // include_Light