#version 330
// Fragment Shader, GLSL v3.30


// INPUTS
uniform float u_time;
uniform sampler2D u_colorTexture;
uniform sampler2D u_depthTexture;
in vec4 a_screenPosition;
in vec4 a_worldPosition;
in vec4 a_surfaceColor;
in vec2 a_textureCoordinates;


// OUTPUTS
out vec4 a_fragColor;


// CONSTANTS
const float PERLIN_SAMPLE_SIZE_X = 0.003;
const float PERLIN_SAMPLE_SIZE_Y = 0.003;
const float ONE_OVER_PERLIN_SAMPLE_SIZE_X = 1.0 / PERLIN_SAMPLE_SIZE_X;
const float ONE_OVER_PERLIN_SAMPLE_SIZE_Y = 1.0 / PERLIN_SAMPLE_SIZE_Y;


//-----------------------------------------------------------------------------------------------
float SmoothStep( float val )
{
	return ( 3.0 * ( val * val ) ) - ( 2.0 * ( val * val * val ) );
}


//-----------------------------------------------------------------------------------------------
float GetRandomNoiseValue( float x, float y )
{
	int n = int( x + ( y * 57 ) );
	n = ( n<<13 )^n;
	return ( 1.0 - ( ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0 );
}


//-----------------------------------------------------------------------------------------------
float PerlinNoise( float posX, float posY )
{
	float southCoord = floor( posX * ONE_OVER_PERLIN_SAMPLE_SIZE_X );
	float northCoord = southCoord + 1.0;
	float eastCoord = floor( posY * ONE_OVER_PERLIN_SAMPLE_SIZE_Y );
	float westCoord = eastCoord + 1.0;
	float percentAcrossX = ( posX * ONE_OVER_PERLIN_SAMPLE_SIZE_X ) - southCoord;
	float percentAcrossY = ( posY * ONE_OVER_PERLIN_SAMPLE_SIZE_Y ) - eastCoord;
	percentAcrossX = SmoothStep( percentAcrossX );
	percentAcrossY = SmoothStep( percentAcrossY );

	vec2 posSE = vec2( southCoord, eastCoord );
	vec2 posSW = vec2( southCoord, westCoord );
	vec2 posNE = vec2( northCoord, eastCoord );
	vec2 posNW = vec2( northCoord, westCoord );

	float theta0 = GetRandomNoiseValue( posSE.x, posSE.y );
	float theta1 = GetRandomNoiseValue( posSW.x, posSW.y );
	float theta2 = GetRandomNoiseValue( posNE.x, posNE.y );
	float theta3 = GetRandomNoiseValue( posNW.x, posNW.y );

	vec2 vecSE = vec2( ( 1.0 - percentAcrossX ), ( 1.0 - percentAcrossY ) );
	vec2 vecSW = vec2( ( 1.0 - percentAcrossX ), percentAcrossY );
	vec2 vecNE = vec2( percentAcrossX, ( 1.0 - percentAcrossY ) );
	vec2 vecNW = vec2( percentAcrossX, percentAcrossY );

	normalize( vecSE );
	normalize( vecSW );
	normalize( vecNE );
	normalize( vecNW );

	float se = dot( vec2( cos( theta0 ), sin( theta0 ) ), vecSE );
	float sw = dot( vec2( cos( theta1 ), sin( theta1 ) ), vecSW );
	float ne = dot( vec2( cos( theta2 ), sin( theta2 ) ), vecNE );
	float nw = dot( vec2( cos( theta3 ), sin( theta3 ) ), vecNW );

	float south = ( se * ( 1.0 - percentAcrossY ) ) + ( sw * percentAcrossY );
	float north = ( ne * ( 1.0 - percentAcrossY ) ) + ( nw * percentAcrossY );

	return ( south * ( 1.0 - percentAcrossX ) ) + ( north * percentAcrossX );
}


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
	vec4 colorTexel = texture2D( u_colorTexture, a_textureCoordinates );
	a_fragColor = ( colorTexel * a_surfaceColor );

	float noise = PerlinNoise( a_textureCoordinates.x + GetRandomNoiseValue( u_time, u_time ), a_textureCoordinates.y + GetRandomNoiseValue( u_time, u_time ) );

	vec4 greyColor = GetAsGreyscale( a_fragColor );
	a_fragColor = greyColor;

	if( noise > 0.999 )
		a_fragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
}