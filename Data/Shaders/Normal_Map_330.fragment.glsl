#version 330
// Fragment Shader, GLSL v3.30


//-----------------------------------------------------------------------------------------------
struct Light
{
	vec3	m_position;
	vec3	m_direction;
	vec4	m_colorAndBrightnessAlpha;
	float	m_innerRadius;
	float	m_outerRadius;
	float	m_innerApertureDot;
	float	m_outerApertureDot;
	float	m_fractionAmbient;
	int		m_isPositionless;
};


//-----------------------------------------------------------------------------------------------
struct DiffAndSpecLightColors
{
	vec4 m_diffuseLightColor;
	vec4 m_specularLightColor;
};


// CONSTANTS
const int MAX_NUMBER_OF_LIGHTS = 16;
const float MAX_SPEC_POWER = 32.0;
const float MAX_FLOAT_VALUE = pow( 10.0, 19.02725 );
const float BUMP_HEIGHT_PERCENTAGE = 0.03;


// INPUTS
uniform int u_numberOfLights;
uniform vec3 u_cameraPosition;
uniform vec4 u_fogColorAndThicknessAlpha;
uniform float u_fogStartDistance;
uniform float u_fogEndDistance;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_specularTexture;
uniform sampler2D u_emissiveTexture;
uniform sampler2D u_depthTexture;
uniform Light u_lights[ MAX_NUMBER_OF_LIGHTS ];
in vec4 a_screenPosition;
in vec4 a_worldPosition;
in vec4 a_surfaceColor;
in vec4 a_surfaceNormal;
in vec4 a_surfaceTangent;
in vec4 a_surfaceBitangent;
in vec2 a_textureCoordinates;


// OUTPUTS
out vec4 a_fragColor;


//-----------------------------------------------------------------------------------------------
float Clamp( float val, float lowest, float highest )
{
	if( val < lowest )
		return lowest;

	if( val > highest )
		return highest;

	return val;
}


//-----------------------------------------------------------------------------------------------
vec4 Clamp( const vec4 vec, float lowest, float highest )
{
	Clamp( vec.x, lowest, highest );
	Clamp( vec.y, lowest, highest );
	Clamp( vec.z, lowest, highest );
	Clamp( vec.w, lowest, highest );

	return vec;
}


//-----------------------------------------------------------------------------------------------
float SmoothStep( float val )
{
	return ( 3.0 * ( val * val ) ) - ( 2.0 * ( val * val * val ) );
}


//-----------------------------------------------------------------------------------------------
vec3 EncodeVectorAsRGBColor( const vec3 vector )
{
	return ( vector + 1.0 ) * 0.5;
}


//-----------------------------------------------------------------------------------------------
vec3 EncodeRGBColorAsVector( const vec3 color )
{
	return ( color * 2.0 ) - 1.0;
}


//-----------------------------------------------------------------------------------------------
vec3 GetNormalInWorldSpace( const vec3 normalInTangentSpace )
{
	mat3 TBNMatrix = mat3( a_surfaceTangent.xyz, a_surfaceBitangent.xyz, a_surfaceNormal.xyz );

	return TBNMatrix * normalInTangentSpace;
}


//-----------------------------------------------------------------------------------------------
vec3 GetNormalInTangentSpace( const vec3 normalInWorldSpace )
{
	mat3 TBNMatrix = mat3( a_surfaceTangent.xyz, a_surfaceBitangent.xyz, a_surfaceNormal.xyz );

	return normalInWorldSpace * TBNMatrix;
}


//-----------------------------------------------------------------------------------------------
vec3 GetActualLightPosition( Light light )
{
	return ( ( 1.0 - float(light.m_isPositionless) ) * light.m_position ) - ( float(light.m_isPositionless) * light.m_direction * MAX_FLOAT_VALUE );
}


//-----------------------------------------------------------------------------------------------
float GetLocalLightStrengthPercentage( Light light )
{
	float lightStrengthPercentage = 1.0;
	vec3 lightPosition = GetActualLightPosition( light );

	float pointToLightDistance = distance( lightPosition, a_worldPosition.xyz );
	float radiusStrengthPercentage = Clamp( ( ( light.m_outerRadius - pointToLightDistance ) / ( light.m_outerRadius - light.m_innerRadius ) ), 0.0, 1.0 );

	if( light.m_innerRadius != 0.0 || light.m_outerRadius != 0.0 )
	{
		lightStrengthPercentage *= radiusStrengthPercentage;
	}

	vec3 lightToPoint = normalize( a_worldPosition.xyz - lightPosition );
	vec3 lightDirection = normalize( light.m_direction );
	float pointDotProduct = dot( lightToPoint, lightDirection );
	float penumbraStrengthPercentage = Clamp( ( ( pointDotProduct - light.m_outerApertureDot ) / ( light.m_innerApertureDot - light.m_outerApertureDot ) ), 0.0, 1.0 );
	lightStrengthPercentage *= penumbraStrengthPercentage;

	return SmoothStep( lightStrengthPercentage );
}


//-----------------------------------------------------------------------------------------------
DiffAndSpecLightColors GetDiffuseAndSpecularLightValues( Light light, vec2 texCoords )
{
	// get texels
	vec4 diffuseTexel = texture2D( u_diffuseTexture, texCoords );
	vec4 normalTexel = texture2D( u_normalTexture, texCoords );
	vec4 specularTexel = texture2D( u_specularTexture, texCoords );

	// actual light position
	vec3 lightPosition = GetActualLightPosition( light );

	// diffuse light with normal mapping
	vec3 normalTexelAsVector = EncodeRGBColorAsVector( normalTexel.xyz );
	vec3 pointToLight = normalize( lightPosition - a_worldPosition.xyz );
	vec3 normalWorldSpace = GetNormalInWorldSpace( normalTexelAsVector );
	float diffuseCoefficient = Clamp( dot( normalWorldSpace, pointToLight ), 0.0, 1.0 );
	vec4 diffuseLightColor = diffuseTexel * a_surfaceColor * light.m_colorAndBrightnessAlpha * diffuseCoefficient;
	if( light.m_fractionAmbient != 0.f )
	{
		diffuseLightColor = diffuseTexel * a_surfaceColor * light.m_colorAndBrightnessAlpha * light.m_fractionAmbient;
	}

	// specular light
	vec3 incidenceVector = -pointToLight;
	vec3 reflectionVector = reflect( incidenceVector, normalWorldSpace );
	vec3 pointToCamera = normalize( u_cameraPosition.xyz - a_worldPosition.xyz );
	float angleBetweenLightAndCamera = max( 0.0, dot( pointToCamera, reflectionVector ) );
	float specularCoefficient = pow( angleBetweenLightAndCamera, 1 + ( MAX_SPEC_POWER * specularTexel.b ) );
	if( diffuseCoefficient == 0.0 )
	{
		specularCoefficient = 0.0;
	}
	vec4 specularLightColor = specularTexel * specularTexel.r * light.m_colorAndBrightnessAlpha * specularCoefficient;

	DiffAndSpecLightColors lightColors;
	float brightness = Clamp( light.m_colorAndBrightnessAlpha.a, 0.0, 1.0 );
	lightColors.m_diffuseLightColor = diffuseLightColor * brightness;
	lightColors.m_specularLightColor = specularLightColor * brightness;
	return lightColors;
}


//-----------------------------------------------------------------------------------------------
void main()
{
	// depth map adjustment
	float height = texture2D( u_depthTexture, a_textureCoordinates ).r;
	vec3 pointToCameraTangentSpace = GetNormalInTangentSpace( normalize( u_cameraPosition.xyz - a_worldPosition.xyz ) );
	vec2 adjustedTexCoords = a_textureCoordinates + ( height * BUMP_HEIGHT_PERCENTAGE * pointToCameraTangentSpace.xy );
	
	// set lights
	vec4 diffuseLightColor = vec4( 0.0, 0.0, 0.0, 0.0 );
	vec4 specularLightColor = vec4( 0.0, 0.0, 0.0, 0.0 );
	for( int lightIndex = 0; lightIndex < u_numberOfLights; ++lightIndex )
	{
		Light light = u_lights[ lightIndex ];
		DiffAndSpecLightColors lightColors = GetDiffuseAndSpecularLightValues( light, adjustedTexCoords );
		float lightStrength = GetLocalLightStrengthPercentage( light );
		diffuseLightColor += lightColors.m_diffuseLightColor * lightStrength;
		specularLightColor += lightColors.m_specularLightColor * lightStrength;
	}
	vec4 totalDiffuseLight = Clamp( diffuseLightColor, 0.0, 1.0 );
	vec4 totalSpecularLight = Clamp( specularLightColor, 0.0, 1.0 );

	// emissive light
	vec4 emissiveTexel = texture2D( u_emissiveTexture, adjustedTexCoords );
	vec4 emissiveLightColor = emissiveTexel;

	// combine light values
	vec4 lightColor = totalDiffuseLight + totalSpecularLight + emissiveLightColor;

	// fog strength
	float pointToCameraDistance = distance( u_cameraPosition.xyz, a_worldPosition.xyz );
	float fogFraction = Clamp( ( pointToCameraDistance - u_fogStartDistance ) / ( u_fogEndDistance - u_fogStartDistance ), 0.0, 1.0 );
	float fogStrength = fogFraction * u_fogColorAndThicknessAlpha.a;
	vec4 fogColor = vec4( u_fogColorAndThicknessAlpha.rgb, 1.0 );

	// final color
	a_fragColor = ( lightColor * ( 1.0 - fogStrength ) ) + ( fogColor * fogStrength );
	a_fragColor.w = 1.0;
}