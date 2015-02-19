#version 330
// Fragment Shader, GLSL v3.30


// INPUTS
uniform sampler2D u_diffuseTexture;
in vec4 a_screenPosition;
in vec4 a_worldPosition;
in vec4 a_surfaceColor;
in vec2 a_textureCoordinates;


// OUTPUTS
out vec4 a_fragColor;


//-----------------------------------------------------------------------------------------------
float CalcLuminance( vec4 color )
{
	vec3 luminanceCoefficients = vec3( 0.2126, 0.7152, 0.0722 );
	float luminance = dot( luminanceCoefficients, color.rgb );
	return luminance;
}


//-----------------------------------------------------------------------------------------------
vec4 GetAsGreyscale( vec4 color )
{
	float luminance = CalcLuminance( color );
	return vec4( luminance, luminance, luminance, color.a );
}


//-----------------------------------------------------------------------------------------------
void main()
{
	vec4 diffuseTexel = texture2D( u_diffuseTexture, a_textureCoordinates );
	a_fragColor = ( diffuseTexel * a_surfaceColor );

	vec4 greyColor = GetAsGreyscale( a_fragColor );
	a_fragColor = greyColor;
}