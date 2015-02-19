#include "World.hpp"
#include "../Engine/MathFunctions.hpp"
#include "../Engine/DeveloperConsole.hpp"


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionChangeShader( const ConsoleCommandArgs& args )
{
	if( args.m_argsList.size() == 0 )
		return false;

	if( args.m_argsList[ 0 ] == "simple" )
	{
		g_world.m_currentMaterial = g_world.m_simpleTextureMaterial;
		return true;
	}

	if( args.m_argsList[ 0 ] == "grayscale" )
	{
		g_world.m_currentMaterial = g_world.m_grayscaleMaterial;
		return true;
	}

	if( args.m_argsList[ 0 ] == "normalMap" )
	{
		g_world.m_currentMaterial = g_world.m_normalMapMaterial;
		return true;
	}

	if( args.m_argsList[ 0 ] == "toon" )
	{
		g_world.m_currentMaterial = g_world.m_toonMaterial;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionChangeFBO( const ConsoleCommandArgs& args )
{
	if( args.m_argsList.size() == 0 )
		return false;

	if( args.m_argsList[ 0 ] == "simple" )
	{
		g_world.m_currentFBOid = g_world.m_FBOid;
		g_world.m_currentFBOMaterial = g_world.m_FBOMaterial;
		return true;
	}

	if( args.m_argsList[ 0 ] == "blur" )
	{
		g_world.m_currentFBOid = g_world.m_blurFBOid;
		g_world.m_currentFBOMaterial = g_world.m_blurFBOMaterial;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionChangeBlurScale( const ConsoleCommandArgs& args )
{
	if( args.m_argsList.size() == 0 )
		return false;

	float blurScale = (float) atof( args.m_argsList[0].c_str() );

	if( blurScale >= 1.f )
	{
		g_world.m_blurScale = blurScale;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
World::World( float worldWith, float worldHeight ) : m_size( worldWith, worldHeight)
{
	
}


//-----------------------------------------------------------------------------------------------
Material* World::InitalizeFBOAndGetMaterial( unsigned int* FBOid, const std::string& vertexShader, const std::string& fragmentShader )
{
	unsigned int framebufferColorID;
	unsigned int framebufferDepthID;

	m_glRenderer.CreateColorFramebufferTexture2D( &framebufferColorID, TEXTURE0, (int) m_size.x, (int) m_size.y );
	m_glRenderer.CreateDepthFramebufferTexture2D( &framebufferDepthID, TEXTURE1, (int) m_size.x, (int) m_size.y );
	m_glRenderer.SetFBO( FBOid, framebufferColorID, framebufferDepthID );

	Texture* colorTexture = new Texture( "" );
	colorTexture->m_openglTextureID = framebufferColorID;
	Texture* depthTexture = new Texture( "" );
	depthTexture->m_openglTextureID = framebufferDepthID;

	Material* FBOMaterial = new Material( vertexShader, fragmentShader );
	FBOMaterial->AddTexture( colorTexture, "u_colorTexture", TEXTURE0 );
	FBOMaterial->AddTexture( depthTexture, "u_depthTexture", TEXTURE1 );

	return FBOMaterial;
}


//-----------------------------------------------------------------------------------------------
void World::Initialize()
{
	m_camera.m_position = Vector3( 0.f, 0.f, 1.f );
	m_monitorView.m_orientation = EulerAngles( 0.f, 90.f, 0.f );

	m_diffuseTexture = Texture::GetTextureByName( "Data/Images/cobblestonesDiffuse.png" );
	m_normalTexture = Texture::GetTextureByName( "Data/Images/cobblestonesNormal.png" );
	m_specularTexture = Texture::GetTextureByName( "Data/Images/cobblestonesSpecular.png" );
	m_emissiveTexture = Texture::GetTextureByName( "Data/Images/cobblestonesEmissive.png" );
	m_depthTexture = Texture::GetTextureByName( "Data/Images/cobblestonesDepth.png" );
	m_obamaTexture = Texture::GetTextureByName( "Data/Images/Mushroom.png" );

	m_simpleTextureMaterial = new Material( SIMPLE_TEXTURE_VERTEX_SHADER_STRING, SIMPLE_TEXTURE_FRAGMENT_SHADER_STRING );
	m_simpleTextureMaterial->AddTexture( m_obamaTexture, "u_diffuseTexture", TEXTURE0 );

	m_simpleNoTextureMaterial = new Material( SIMPLE_NO_TEXTURE_VERTEX_SHADER_STRING, SIMPLE_NO_TEXTURE_FRAGMENT_SHADER_STRING );

	m_grayscaleMaterial = new Material( GRAYSCALE_VERTEX_SHADER_STRING, GRAYSCALE_FRAGMENT_SHADER_STRING );
	m_grayscaleMaterial->AddTexture( m_diffuseTexture, "u_diffuseTexture", TEXTURE0 );

	m_normalMapMaterial = new Material( NORMAL_MAP_VERTEX_SHADER_STRING, NORMAL_MAP_FRAGMENT_SHADER_STRING );
	m_normalMapMaterial->AddTexture( m_diffuseTexture, "u_diffuseTexture", TEXTURE0 );
	m_normalMapMaterial->AddTexture( m_normalTexture, "u_normalTexture", TEXTURE1 );
	m_normalMapMaterial->AddTexture( m_specularTexture, "u_specularTexture", TEXTURE2 );
	m_normalMapMaterial->AddTexture( m_emissiveTexture, "u_emissiveTexture", TEXTURE3 );
	m_normalMapMaterial->AddTexture( m_depthTexture, "u_depthTexture", TEXTURE4 );

	m_toonMaterial = new Material( TOON_VERTEX_SHADER_STRING, TOON_FRAGMENT_SHADER_STRING );
	m_toonMaterial->AddTexture( m_obamaTexture, "u_diffuseTexture", TEXTURE0 );

	m_currentMaterial = m_normalMapMaterial;

	g_developerConsole.AddCommandFuncPtr( "changeShader", ConsoleFunctionChangeShader );
	g_developerConsole.AddCommandFuncPtr( "changeFBO", ConsoleFunctionChangeFBO );
	g_developerConsole.AddCommandFuncPtr( "changeBlurScale", ConsoleFunctionChangeBlurScale );

	m_lightPointLifeSeconds = 0.f;
	SetSimpleLightScene();

	SetVertices();
	m_glRenderer.SetVBO( &m_VBOid, m_triangleVertices );
	m_glRenderer.SetVBO( &m_screenVBOid, m_screenVertices );
	m_glRenderer.SetVBO( &m_monitorVBOid, m_monitorVertices );

	m_FBOMaterial = InitalizeFBOAndGetMaterial( &m_FBOid, FBO_VERTEX_SHADER_STRING, FBO_FRAGMENT_SHADER_STRING );
	m_blurFBOMaterial = InitalizeFBOAndGetMaterial( &m_blurFBOid, BLUR_FBO_VERTEX_SHADER_STRING, BLUR_FBO_FRAGMENT_SHADER_STRING );
	m_monitorFBOMaterial = InitalizeFBOAndGetMaterial( &m_monitorFBOid, MONITOR_FBO_VERTEX_SHADER_STRING, MONITOR_FBO_FRAGMENT_SHADER_STRING );

	m_blurScale = INITIAL_BLUR_SCALE;

	m_currentFBOMaterial = m_FBOMaterial;
	m_currentFBOid = m_FBOid;

	m_glRenderer.DisableFramebuffer();
}


//-----------------------------------------------------------------------------------------------
void World::Deconstruct()
{
	delete m_diffuseTexture;
	delete m_normalTexture;
}


//-----------------------------------------------------------------------------------------------
bool World::ProcessKeyDownEvent( unsigned char virtualKeyCode )
{
	m_keyboard.SetKeyDown( virtualKeyCode );
	return false;
}


//-----------------------------------------------------------------------------------------------
bool World::ProcessKeyUpEvent( unsigned char virtualKeyCode )
{
	m_keyboard.SetKeyUp( virtualKeyCode );
	return false;
}


//-----------------------------------------------------------------------------------------------
bool World::ProcessCharDownEvent( unsigned char charCode )
{
	m_keyboard.SetCharDown( charCode );
	return false;
}


//-----------------------------------------------------------------------------------------------
void World::SetCameraPositionAndOrientation( const Camera& camera )
{
	m_glRenderer.Rotatef( -90.f, 1.f, 0.f, 0.f );
	m_glRenderer.Rotatef( 90.f, 0.f, 0.f, 1.f );

	m_glRenderer.Rotatef( -camera.m_orientation.roll, 1.f, 0.f, 0.f );
	m_glRenderer.Rotatef( -camera.m_orientation.pitch, 0.f, 1.f, 0.f );
	m_glRenderer.Rotatef( -camera.m_orientation.yaw, 0.f, 0.f, 1.f );

	m_glRenderer.Translatef( -camera.m_position.x, -camera.m_position.y, -camera.m_position.z );
}


//-----------------------------------------------------------------------------------------------
void World::CircleTable( float** sint, float** cost, const int n )
{
	int i;

	/* Table size, the sign of n flips the circle direction */

	const int size = abs(n);

	/* Determine the angle between samples */

	const float angle = ( 2.f * pi ) / (float) n;

	/* Allocate memory for n samples, plus duplicate of first entry at the end */

	*sint = (float*) calloc( sizeof(float), size + 1 );
	*cost = (float*) calloc( sizeof(float), size + 1 );

	/* Bail out if memory allocation fails, fgError never returns */

	if ( !(*sint) || !(*cost) )
	{
		free( *sint );
		free( *cost );
	}

	/* Compute cos and sin around the circle */

	for ( i=0; i<size; i++ )
	{
		(*sint)[i] = sin( angle*i );
		(*cost)[i] = cos( angle*i );
	}

	/* Last sample is duplicate of the first */

	(*sint)[size] = (*sint)[0];
	(*cost)[size] = (*cost)[0];
}


//-----------------------------------------------------------------------------------------------
void World::SetCubeVertices()
{
	float xMin = CUBE_CENTER_POSITION.x - CUBE_X_AXIS_HALF_LENGTH;
	float xMax = CUBE_CENTER_POSITION.x + CUBE_X_AXIS_HALF_LENGTH;
	float yMin = CUBE_CENTER_POSITION.y - CUBE_Y_AXIS_HALF_LENGTH;
	float yMax = CUBE_CENTER_POSITION.y + CUBE_Y_AXIS_HALF_LENGTH;
	float zMin = CUBE_CENTER_POSITION.z - CUBE_Z_AXIS_HALF_LENGTH;
	float zMax = CUBE_CENTER_POSITION.z + CUBE_Z_AXIS_HALF_LENGTH;

	Vertex vert0;
	Vertex vert1;
	Vertex vert2;

	vert0 = Vertex( Vector3( xMin, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert1 = Vertex( Vector3( xMin, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert2 = Vertex( Vector3( xMin, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMin, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert1 = Vertex( Vector3( xMin, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert2 = Vertex( Vector3( xMin, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( -1.f, 0.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert1 = Vertex( Vector3( xMax, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert2 = Vertex( Vector3( xMax, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert1 = Vertex( Vector3( xMax, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert2 = Vertex( Vector3( xMax, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMin, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert1 = Vertex( Vector3( xMax, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert2 = Vertex( Vector3( xMin, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert1 = Vertex( Vector3( xMin, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert2 = Vertex( Vector3( xMax, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, -1.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert1 = Vertex( Vector3( xMin, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert2 = Vertex( Vector3( xMax, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMin, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert1 = Vertex( Vector3( xMax, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert2 = Vertex( Vector3( xMin, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 1.f, 0.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMin, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert1 = Vertex( Vector3( xMin, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert2 = Vertex( Vector3( xMax, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert1 = Vertex( Vector3( xMax, yMin, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert2 = Vertex( Vector3( xMin, yMax, zMin ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, -1.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMin, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert1 = Vertex( Vector3( xMin, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert2 = Vertex( Vector3( xMax, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );

	vert0 = Vertex( Vector3( xMax, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert1 = Vertex( Vector3( xMax, yMax, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert2 = Vertex( Vector3( xMin, yMin, zMax ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, 1.f ) );
	vert0.SetTangent( vert2, vert1 );
	vert1.SetTangent( vert0, vert2 );
	vert2.SetTangent( vert1, vert0 );
	m_triangleVertices.push_back( vert0 );
	m_triangleVertices.push_back( vert1 );
	m_triangleVertices.push_back( vert2 );
}


//-----------------------------------------------------------------------------------------------
void World::SetSphereVertices()
{
	/* Adjust z and radius as stacks are drawn. */

	float z0, z1;
	float r0, r1;

	/* Pre-computed circle */

	float *sint1, *cost1;
	float *sint2, *cost2;
	CircleTable( &sint1, &cost1, -SPHERE_NUM_SLICES );
	CircleTable( &sint2, &cost2, SPHERE_NUM_STACKS * 2 );

	/* The top stack is covered with a triangle fan */

	z0 = 1.f;
	z1 = cost2[1];
	r0 = 0.f;
	r1 = sint2[1];

	Vertex vert0;
	Vertex vert1;
	Vertex vert2;

	for( int sliceIndex = 0; sliceIndex < SPHERE_NUM_SLICES; ++sliceIndex )
	{
		vert0 = Vertex( Vector3( SPHERE_CENTER_POSITION.x, SPHERE_CENTER_POSITION.y, SPHERE_CENTER_POSITION.z + SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f, 0.5f ), Vector3( 0.f, 0.f, 1.f ) );
		vert1 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex + 1 ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex + 1 ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z1 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex + 1 ] * r1 ), 0.5f + 0.5f * ( sint1[ sliceIndex + 1 ] * r1 ) ), Vector3( cost1[ sliceIndex + 1 ] * r1, sint1[ sliceIndex + 1 ] * r1, z1 ) );
		vert2 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z1 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex ] * r1 ), 0.5f + 0.5f * ( sint1[ sliceIndex ] * r1 ) ), Vector3( cost1[ sliceIndex ] * r1, sint1[ sliceIndex ] * r1, z1 ) );
		vert0.SetTangent( vert2, vert1 );
		vert1.SetTangent( vert0, vert2 );
		vert2.SetTangent( vert1, vert0 );
		m_triangleVertices.push_back( vert0 );
		m_triangleVertices.push_back( vert1 );
		m_triangleVertices.push_back( vert2 );
	}

	/* Cover each stack with a quad strip, except the top and bottom stacks */

	for( int stackIndex = 1; stackIndex < SPHERE_NUM_STACKS - 1; ++stackIndex )
	{
		z0 = z1;
		z1 = cost2[ stackIndex + 1 ];
		r0 = r1;
		r1 = sint2[ stackIndex + 1 ];

		for( int sliceIndex = 0; sliceIndex < SPHERE_NUM_SLICES; ++sliceIndex )
		{
			vert0 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z1 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex ] * r1 ), 0.5f + 0.5f * ( sint1[ sliceIndex ] * r1 ) ), Vector3( cost1[ sliceIndex ] * r1, sint1[ sliceIndex ] * r1, z1 ) );
			vert1 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z0 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex ] * r0 ), 0.5f + 0.5f * ( sint1[ sliceIndex ] * r0 ) ), Vector3( cost1[ sliceIndex ] * r0, sint1[ sliceIndex ] * r0, z0 ) );
			vert2 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z0 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex + 1 ] * r0 ), 0.5f + 0.5f * ( sint1[ sliceIndex + 1 ] * r0 ) ), Vector3( cost1[ sliceIndex + 1 ] * r0, sint1[ sliceIndex + 1 ] * r0, z0 ) );
			vert0.SetTangent( vert2, vert1 );
			vert1.SetTangent( vert0, vert2 );
			vert2.SetTangent( vert1, vert0 );
			m_triangleVertices.push_back( vert0 );
			m_triangleVertices.push_back( vert1 );
			m_triangleVertices.push_back( vert2 );

			vert0 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z1 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex ] * r1 ), 0.5f + 0.5f * ( sint1[ sliceIndex ] * r1 ) ), Vector3( cost1[ sliceIndex ] * r1, sint1[ sliceIndex ] * r1, z1 ) );
			vert1 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z0 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex + 1 ] * r0 ), 0.5f + 0.5f * ( sint1[ sliceIndex + 1 ] * r0 ) ), Vector3( cost1[ sliceIndex + 1 ] * r0, sint1[ sliceIndex + 1 ] * r0, z0 ) );
			vert2 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex + 1 ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex + 1 ] * r1 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z1 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex + 1 ] * r1 ), 0.5f + 0.5f * ( sint1[ sliceIndex + 1 ] * r1 ) ), Vector3( cost1[ sliceIndex + 1 ] * r1, sint1[ sliceIndex + 1 ] * r1, z1 ) );
			vert0.SetTangent( vert2, vert1 );
			vert1.SetTangent( vert0, vert2 );
			vert2.SetTangent( vert1, vert0 );
			m_triangleVertices.push_back( vert0 );
			m_triangleVertices.push_back( vert1 );
			m_triangleVertices.push_back( vert2 );
		}
	}

	/* The bottom stack is covered with a triangle fan */

	z0 = z1;
	r0 = r1;

	for( int sliceIndex = 0; sliceIndex < SPHERE_NUM_SLICES; ++sliceIndex )
	{
		vert0 = Vertex( Vector3( SPHERE_CENTER_POSITION.x, SPHERE_CENTER_POSITION.y, SPHERE_CENTER_POSITION.z - SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f, 0.5f ), Vector3( 0.f, 0.f, -1.f ) );
		vert1 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z0 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex ] * r0 ), 0.5f + 0.5f * ( sint1[ sliceIndex ] * r0 ) ), Vector3( cost1[ sliceIndex ] * r0, sint1[ sliceIndex ] * r0, z0 ) );
		vert2 = Vertex( Vector3( SPHERE_CENTER_POSITION.x + cost1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.y + sint1[ sliceIndex + 1 ] * r0 * SPHERE_RADIUS, SPHERE_CENTER_POSITION.z + z0 * SPHERE_RADIUS ), Color( 1.f, 1.f, 1.f, 1.f ), Vector2( 0.5f + 0.5f * ( cost1[ sliceIndex + 1 ] * r0 ), 0.5f + 0.5f * ( sint1[ sliceIndex + 1 ] * r0 ) ), Vector3( cost1[ sliceIndex + 1 ] * r0, sint1[ sliceIndex + 1 ] * r0, z0 ) );
		vert0.SetTangent( vert2, vert1 );
		vert1.SetTangent( vert0, vert2 );
		vert2.SetTangent( vert1, vert0 );
		m_triangleVertices.push_back( vert0 );
		m_triangleVertices.push_back( vert1 );
		m_triangleVertices.push_back( vert2 );
	}

	/* Release sin and cos tables */

	free(sint1);
	free(cost1);
	free(sint2);
	free(cost2);
}


//-----------------------------------------------------------------------------------------------
void World::SetScreenVertices()
{
	m_screenVertices.push_back( Vertex( Vector3( 0.f, 0.f, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_screenVertices.push_back( Vertex( Vector3( m_size.x, 0.f, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_screenVertices.push_back( Vertex( Vector3( m_size.x, m_size.y, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );

	m_screenVertices.push_back( Vertex( Vector3( 0.f, m_size.y, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_screenVertices.push_back( Vertex( Vector3( 0.f, 0.f, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_screenVertices.push_back( Vertex( Vector3( m_size.x, m_size.y, 0.f ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void World::SetMonitorVertices()
{
	float screenHalfWidth = 7.f;
	float screenHalfHeight = screenHalfWidth * ( 9.f * ONE_OVER_SIXTEEN );

	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, screenHalfWidth, screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, screenHalfWidth, -screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, -screenHalfWidth, -screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );

	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, -screenHalfWidth, screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, screenHalfWidth, screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 0.f, 1.f ), Vector3( 0.f, 0.f, 0.f ) ) );
	m_monitorVertices.push_back( Vertex( Vector3( MONITOR_POSITION_X, -screenHalfWidth, -screenHalfHeight ), Color( 1.f, 1.f, 1.f ), Vector2( 1.f, 0.f ), Vector3( 0.f, 0.f, 0.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void World::SetVertices()
{
	SetCubeVertices();
	SetSphereVertices();
	SetScreenVertices();
	SetMonitorVertices();
}


//-----------------------------------------------------------------------------------------------
void World::AddLight( const Vector3& position, const Vector3& direction, const Color& colorAndBrightness, float innerRadius, float outerRadius, float innerApertureDot, float outerApertureDot, float fractionAmbient, bool isPositionless )
{
	Light light( position, direction, colorAndBrightness, innerRadius, outerRadius, innerApertureDot, outerApertureDot, fractionAmbient, isPositionless );
	m_lights.push_back( light );
}


//-----------------------------------------------------------------------------------------------
void World::SetLightsInShader()
{
	for( unsigned int lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex )
	{
		Light light = m_lights[ lightIndex ];
		light.SetUniformValues( m_currentMaterial, lightIndex );
	}
}


//-----------------------------------------------------------------------------------------------
void World::SetSimpleLightScene()
{
	m_lights.clear();
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 1.f, 1.f, 1.f, 1.f ), 0.f, 0.f, -1.f, -1.f, 0.f, false );
}


//-----------------------------------------------------------------------------------------------
void World::SetPointLightsScene()
{
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 1.f, 0.f, 0.f, 1.f ), 0.f, 0.f, -1.f, -1.f, 0.f, false );
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 0.f, 1.f, 0.f, 1.f ), 1.f, 2.f, -1.f, -1.f, 0.f, false );
}


//-----------------------------------------------------------------------------------------------
void World::SetSpotlightsScene()
{
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 0.f, 0.f, 1.f, 1.f ), 3.f, 5.f, 0.9f, 0.7f, 0.f, false );
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 1.f, -1.f ), Color( 1.f, 1.f, 0.f, 0.25f ), 0.f, 0.f, 0.7f, 0.6f, 0.f, false );
}


//-----------------------------------------------------------------------------------------------
void World::SetAmbientLightsScene()
{
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 1.f, 0.f, 1.f, 1.f ), 0.f, 0.f, -1.f, -1.f, 0.5f, false );
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 0.f, 0.f, -1.f ), Color( 0.f, 1.f, 1.f, 1.f ), 2.f, 3.f, -1.f, -1.f, 1.f, false );
}


//-----------------------------------------------------------------------------------------------
void World::SetGlobalDirectionalLightScene()
{
	AddLight( Vector3( 0.f, 0.f, 0.f ), Vector3( 1.f, -1.f, -1.f ), Color( 1.f, 1.f, 1.f, 1.f ), 0.f, 0.f, -1.f, -1.f, 0.f, true );
}


//-----------------------------------------------------------------------------------------------
void World::SetAllLightsScene()
{
	SetPointLightsScene();
	SetSpotlightsScene();
	SetAmbientLightsScene();
	SetGlobalDirectionalLightScene();
}


//-----------------------------------------------------------------------------------------------
void World::UpdateCameraPositionFromInput( float deltaSeconds )
{
	float yawRadians = ConvertDegreesToRadians( m_camera.m_orientation.yaw );
	Vector3 cameraMoveVector( 0.f, 0.f, 0.f );
	Vector3 cameraForwardXY( cos( yawRadians ), sin( yawRadians ), 0.f );
	Vector3 cameraLeftXY( -cameraForwardXY.y, cameraForwardXY.x, 0.f );

	if( m_keyboard.IsKeyPressedDown( KEY_W ) )
	{
		cameraMoveVector += cameraForwardXY;
	}
	else if( m_keyboard.IsKeyPressedDown( KEY_S ) )
	{
		cameraMoveVector -= cameraForwardXY;
	}

	if( m_keyboard.IsKeyPressedDown( KEY_A ) )
	{
		cameraMoveVector += cameraLeftXY;
	}
	else if( m_keyboard.IsKeyPressedDown( KEY_D ) )
	{
		cameraMoveVector -= cameraLeftXY;
	}

	if( m_keyboard.IsKeyPressedDown( KEY_E ) )
	{
		cameraMoveVector.z += 1.f;
	}
	else if( m_keyboard.IsKeyPressedDown( KEY_Q ) )
	{
		cameraMoveVector.z -= 1.f;
	}

	cameraMoveVector *= ( MOVE_SPEED_POINTS_PER_SECOND * deltaSeconds );
	m_camera.m_position += cameraMoveVector;
}


//-----------------------------------------------------------------------------------------------
void World::UpdateCameraOrientationFromInput( float deltaSeconds )
{
	float rotationRadiansPerSecond = ConvertDegreesToRadians( ROTATION_DEGREES_PER_SECOND );
	EulerAngles cameraRotation( 0.f, 0.f, 0.f );

	Vector2 currentMousePosition = m_mouse.GetCursorPosition();
	m_mouse.SetCursorPosition( Vector2( ( m_size.x * 0.5f ), ( m_size.y * 0.5f ) ) );

	cameraRotation.yaw += ( m_size.x * 0.5f ) - currentMousePosition.x;
	cameraRotation.pitch += currentMousePosition.y - ( m_size.y * 0.5f );
	cameraRotation *= ( rotationRadiansPerSecond * deltaSeconds );
	m_camera.m_orientation += cameraRotation;

	if( m_camera.m_orientation.pitch > 90.f )
	{
		m_camera.m_orientation.pitch = 90.f;
	}
	else if( m_camera.m_orientation.pitch < -90.f )
	{
		m_camera.m_orientation.pitch = -90.f;
	}
}


//-----------------------------------------------------------------------------------------------
void World::UpdateFromInput( float deltaSeconds )
{
	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_TILDE ) )
	{
		g_developerConsole.m_drawConsole = !g_developerConsole.m_drawConsole;
	}

	UpdateCameraOrientationFromInput( deltaSeconds );

	if( g_developerConsole.m_drawConsole )
		return;

	UpdateCameraPositionFromInput( deltaSeconds );

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_1 ) )
	{
		m_lights.clear();
		SetAllLightsScene();
	}

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_2 ) )
	{
		m_lights.clear();
		SetPointLightsScene();
	}

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_3 ) )
	{
		m_lights.clear();
		SetSpotlightsScene();
	}

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_4 ) )
	{
		m_lights.clear();
		SetAmbientLightsScene();
	}

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_5 ) )
	{
		m_lights.clear();
		SetGlobalDirectionalLightScene();
	}

	if( m_keyboard.IsKeyPressedDownAndWasNotBefore( KEY_6 ) )
	{
		SetSimpleLightScene();
	}
}


//-----------------------------------------------------------------------------------------------
void World::UpdateLights()
{
	for( unsigned int lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex )
	{
		Light* light = &m_lights[ lightIndex ];
		light->UpdatePosition( Vector3( 2.f * sin( m_lightPointLifeSeconds + ( lightIndex * 1.42f ) ), 2.f * sin( 2.f * m_lightPointLifeSeconds + ( lightIndex * 1.42f ) ), 1.f ) );
	}
}


//-----------------------------------------------------------------------------------------------
void World::Update()
{
	float deltaSeconds = static_cast< float >( FRAME_TIME_SECONDS );

	UpdateFromInput( deltaSeconds );
	UpdateLights();

	if( g_developerConsole.m_drawConsole )
		g_developerConsole.Update( m_keyboard, deltaSeconds );

	m_keyboard.Update();
	m_monitorView.m_position = Vector3( 2.f * sin( m_lightPointLifeSeconds ), 2.f * sin( 2.f * m_lightPointLifeSeconds ), 1.f );

	m_lightPointLifeSeconds += deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void World::BindMonitorToFramebuffer()
{
	m_glRenderer.BindFramebuffer( m_monitorFBOid );
	m_glRenderer.PushMatrix();

	m_glRenderer.EnableDepthTest();
	m_glRenderer.ClearColorBufferBit();
	m_glRenderer.ClearDepthBufferBit();

	SetCameraPositionAndOrientation( m_monitorView );

	m_glRenderer.BindBuffer( m_VBOid );
	m_currentMaterial->Enable();
	{
		int numberOfLights = m_lights.size();
		m_currentMaterial->SetUniform( "u_numberOfLights", numberOfLights );
		m_currentMaterial->SetUniform( "u_cameraPosition", m_camera.m_position );
		m_currentMaterial->SetUniform( "u_fogColorAndThicknessAlpha", SHADER_FOG_COLOR );
		m_currentMaterial->SetUniform( "u_fogStartDistance", SHADER_FOG_START_DISTANCE );
		m_currentMaterial->SetUniform( "u_fogEndDistance", SHADER_FOG_END_DISTANCE );
		m_currentMaterial->SetUniform( "u_numberOfShades", 3.f );
		SetLightsInShader();

		m_glRenderer.RenderVertices( m_triangleVertices, TRIANGLES );
		RenderLights();
	}
	m_currentMaterial->Disable();

	m_monitorFBOMaterial->Enable();
	m_monitorFBOMaterial->SetUniform( "u_time", m_lightPointLifeSeconds );
	m_monitorFBOMaterial->Disable();

	m_glRenderer.PopMatrix();
	m_glRenderer.DisableFramebuffer();
}


//-----------------------------------------------------------------------------------------------
void World::RenderLights()
{
	for( unsigned int lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex )
	{
		m_lights[ lightIndex ].Render();
	}
}


//-----------------------------------------------------------------------------------------------
void World::RenderMonitor()
{
	m_glRenderer.PushMatrix();

	m_glRenderer.BindBuffer( m_monitorVBOid );
	m_monitorFBOMaterial->Enable();
	m_glRenderer.RenderVertices( m_monitorVertices, TRIANGLES );
	m_monitorFBOMaterial->Disable();

	m_glRenderer.PopMatrix();
}


//-----------------------------------------------------------------------------------------------
void World::RenderFBO()
{
	m_glRenderer.PushMatrix();

	m_glRenderer.SetClearColor( 0.f, 0.2f, 0.4f, 1.f );
	m_glRenderer.SetClearDepth( 1.f );
	m_glRenderer.ClearColorBufferBit();
	m_glRenderer.ClearDepthBufferBit();
	m_glRenderer.DisableDepthTest();
	m_glRenderer.SetDepthMask( false );
	m_glRenderer.SetOrtho( 0.f, m_size.x, 0.f, m_size.y, 0.f, 1.f );

	m_glRenderer.BindBuffer( m_screenVBOid );
	m_currentFBOMaterial->Enable();
	m_glRenderer.RenderVertices( m_screenVertices, TRIANGLES );
	m_currentFBOMaterial->Disable();

	m_glRenderer.EnableDepthTest();
	m_glRenderer.SetDepthMask( true );

	m_glRenderer.PopMatrix();
}


//-----------------------------------------------------------------------------------------------
void World::RenderObjects3D()
{
	m_glRenderer.PushMatrix();

	m_glRenderer.EnableDepthTest();
	m_glRenderer.ClearColorBufferBit();
	m_glRenderer.ClearDepthBufferBit();

	m_glRenderer.SetPerspective( FIELD_OF_VIEW_Y, SIXTEEN_BY_NINE, 0.1f, 1000.f );

	BindMonitorToFramebuffer();
	m_glRenderer.BindFramebuffer( m_currentFBOid );

	SetCameraPositionAndOrientation( m_camera );

	m_glRenderer.BindBuffer( m_VBOid );
	m_currentMaterial->Enable();
	{
		int numberOfLights = m_lights.size();
		m_currentMaterial->SetUniform( "u_numberOfLights", numberOfLights );
		m_currentMaterial->SetUniform( "u_cameraPosition", m_camera.m_position );
		m_currentMaterial->SetUniform( "u_fogColorAndThicknessAlpha", SHADER_FOG_COLOR );
		m_currentMaterial->SetUniform( "u_fogStartDistance", SHADER_FOG_START_DISTANCE );
		m_currentMaterial->SetUniform( "u_fogEndDistance", SHADER_FOG_END_DISTANCE );
		m_currentMaterial->SetUniform( "u_numberOfShades", 3.f );
		SetLightsInShader();

		m_glRenderer.RenderVertices( m_triangleVertices, TRIANGLES );
		RenderLights();
		RenderMonitor();
	}
	m_currentMaterial->Disable();

	m_currentFBOMaterial->Enable();
	m_currentFBOMaterial->SetUniform( "u_blurAreaDimensions", m_size );
	m_currentFBOMaterial->SetUniform( "u_blurScale", m_blurScale );
	m_currentFBOMaterial->Disable();

	m_glRenderer.PopMatrix();
}


//-----------------------------------------------------------------------------------------------
void World::RenderObjects2D()
{
	m_glRenderer.PushMatrix();

	m_glRenderer.DisableDepthTest();
	m_glRenderer.SetOrtho( 0.f, m_size.x, 0.f, m_size.y, 0.f, 1.f );

	if( g_developerConsole.m_drawConsole )
		g_developerConsole.Render();

	m_glRenderer.PopMatrix();
}


//-----------------------------------------------------------------------------------------------
void World::Render()
{
	m_glRenderer.BindFramebuffer( m_currentFBOid );
	RenderObjects3D();
	m_glRenderer.DisableFramebuffer();
	RenderFBO();
	RenderObjects2D();

	//make a tolower function
}