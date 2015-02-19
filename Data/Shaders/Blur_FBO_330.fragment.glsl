#version 330
// Fragment Shader, GLSL v3.30


// INPUTS
uniform vec2 u_blurAreaDimensions;
uniform float u_blurScale;
uniform sampler2D u_colorTexture;
uniform sampler2D u_depthTexture;
in vec4 a_screenPosition;
in vec4 a_worldPosition;
in vec4 a_surfaceColor;
in vec2 a_textureCoordinates;


// OUTPUTS
out vec4 a_fragColor;


// CONSTANTS
const int KERNEL_SIZE = 25;
const float kernel_weights[KERNEL_SIZE] = float[]
(
	0.00048031, 0.00500493, 0.01093176, 0.00500493, 0.00048031,
	0.00500493, 0.05215252, 0.11391157, 0.05215252, 0.00500493,
	0.01093176, 0.11391157, 0.24880573, 0.11391157, 0.01093176,
	0.00500493, 0.05215252, 0.11391157, 0.05215252, 0.00500493,
	0.00048031, 0.00500493, 0.01093176, 0.00500493, 0.00048031
);
const float weights_factor = 1.01238;


//-----------------------------------------------------------------------------------------------
vec2[KERNEL_SIZE] GetOffsetArray( const vec2 pixelScale )
{
	vec2 offset[KERNEL_SIZE] = vec2[]
	(
		vec2 ( -pixelScale.s * 2.0, -pixelScale.t * 2.0 ),
		vec2 ( -pixelScale.s, -pixelScale.t * 2.0 ),
		vec2 ( 0.0, -pixelScale.t * 2.0 ),
		vec2 ( pixelScale.s, -pixelScale.t * 2.0 ),
		vec2 ( pixelScale.s * 2.0, -pixelScale.t * 2.0 ),
		
		vec2 (	-pixelScale.s * 2.0, -pixelScale.t ),
		vec2 (	-pixelScale.s, -pixelScale.t ),
		vec2 (	0.0, -pixelScale.t ),
		vec2 (	pixelScale.s, -pixelScale.t ),
		vec2 (	pixelScale.s * 2.0, -pixelScale.t ),
		
		vec2 (	-pixelScale.s * 2.0, 0.0 ),
		vec2 (	-pixelScale.s, 0.0 ),
		vec2 (	0.0, 0.0 ),
		vec2 (	pixelScale.s, 0.0 ),
		vec2 (	pixelScale.s * 2.0, 0.0 ),
		
		vec2 (	-pixelScale.s * 2.0, pixelScale.t ),
		vec2 (	-pixelScale.s, pixelScale.t ),
		vec2 (	0.0, pixelScale.t ),
		vec2 (	pixelScale.s, pixelScale.t ),
		vec2 (	pixelScale.s * 2.0, pixelScale.t ),
		
		vec2 (	-pixelScale.s * 2.0, pixelScale.t * 2.0 ),
		vec2 (	-pixelScale.s, pixelScale.t * 2.0 ),
		vec2 (	0.0, pixelScale.t * 2.0 ),
		vec2 (	pixelScale.s, pixelScale.t * 2.0 ),
		vec2 (	pixelScale.s * 2.0, pixelScale.t * 2.0 )
	);

	return offset;
}


//-----------------------------------------------------------------------------------------------
void main()
{
	float depth = texture2D( u_depthTexture, a_textureCoordinates ).r;
	vec2 pixelScale;
	pixelScale.x = u_blurScale * ( 1.0 / u_blurAreaDimensions.x );
	pixelScale.y = u_blurScale * ( 1.0 / u_blurAreaDimensions.y );
	vec2[KERNEL_SIZE] offset = GetOffsetArray( pixelScale );

	vec3 color = vec3( 0.0, 0.0, 0.0 );
	for ( int i = 0; i < KERNEL_SIZE; ++i )
	{
		color += texture2D( u_colorTexture, a_textureCoordinates + offset[i] ).rgb * kernel_weights[i] * weights_factor;
	}

	a_fragColor = vec4( color, 1.0 );
}