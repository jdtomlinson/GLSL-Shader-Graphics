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


// CONSTANTS
const float gamma = 0.6;


//-----------------------------------------------------------------------------------------------
float SmoothStep( float val )
{
	return ( 3.0 * ( val * val ) ) - ( 2.0 * ( val * val * val ) );
}


//-----------------------------------------------------------------------------------------------
float CalcLuminance( vec4 color )
{
	vec3 luminanceCoefficients = vec3( 0.2126, 0.7152, 0.0722 );
	float luminance = dot( luminanceCoefficients, color.rgb );
	return luminance;
}


//-----------------------------------------------------------------------------------------------
void main()
{
	float numColors = 4.0;
	vec4 diffuseTexel = texture2D( u_diffuseTexture, a_textureCoordinates );
	float lum = CalcLuminance( diffuseTexel );
	vec3 c = vec3( lum, lum, lum );

	c = pow(c, vec3(gamma, gamma, gamma));
	c = c * numColors;
	c = floor(c);
	c = clamp( ( c + 1 ) / numColors, 0.0, 1.0 );
	

	if ( c.r == ( ( numColors ) / numColors ) )
	{
		a_fragColor = vec4( 1.0, 0.88, 0.56, 1.0 );
	}
	else if ( c.r == ( ( numColors - 1 ) / numColors ) )
	{
		a_fragColor = vec4( 0.31, 0.57, 0.62, 1.0 );
	}
	else if ( c.r == ( ( numColors - 2 ) / numColors ) )
	{
		a_fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
	}
	else
	{
		a_fragColor = vec4( 0.0, 0.15, 0.24, 1.0 );
	}

	//a_fragColor = diffuseTexel * vec4( c, 1.0 );
}