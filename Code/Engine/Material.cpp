#include "Material.hpp"


//-----------------------------------------------------------------------------------------------
Material::Material( const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName )
{
	m_glRenderer = OpenGLRenderer();
	m_glShaderProgram = new OpenGLShaderProgram( vertexShaderFileName, fragmentShaderFileName );
}


//-----------------------------------------------------------------------------------------------
Material::~Material()
{
	delete m_glShaderProgram;
}


//-----------------------------------------------------------------------------------------------
void Material::Enable()
{
	m_glRenderer.UseShaderProgram( m_glShaderProgram->m_programID );

	BindTextures();
	SendUniformValues();
	SendAttribValues();
	EnableAttribArrays();
}


//-----------------------------------------------------------------------------------------------
void Material::Disable()
{
	m_glRenderer.UseShaderProgram( 0 );

	m_glRenderer.DisableAttribArray( VERTEX_ATTRIB_POSITIONS );
	m_glRenderer.DisableAttribArray( VERTEX_ATTRIB_COLORS );
	m_glRenderer.DisableAttribArray( VERTEX_ATTRIB_TEX_COORDS );
	m_glRenderer.DisableAttribArray( VERTEX_ATTRIB_NORMALS );
	m_glRenderer.DisableAttribArray( VERTEX_ATTRIB_TANGENTS );
}


//-----------------------------------------------------------------------------------------------
void Material::AddTexture( Texture* texture, std::string textureUniformName, textureUnit activeTextureLayer )
{
	int uniformLocation = m_glRenderer.GetUniformLocation( m_glShaderProgram->m_programID, textureUniformName.c_str() );
	TextureInfo textureInfo( texture, uniformLocation, activeTextureLayer );
	m_textures.push_back( textureInfo );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, int val )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform1i( uniformLocation, val );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, bool isTrue )
{
	int uniformLocation = GetUniformLocation( uniformName );

	if( isTrue )
		m_glRenderer.SetUniform1i( uniformLocation, 1 );
	else
		m_glRenderer.SetUniform1i( uniformLocation, 0 );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, float val )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform1f( uniformLocation, val );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, const Vector2& vec )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform2f( uniformLocation, vec.x, vec.y );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, const Vector3& vec )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform3f( uniformLocation, vec.x, vec.y, vec.z );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, const Vector4& vec )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform4f( uniformLocation, vec.x, vec.y, vec.z, vec.w );
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( const std::string& uniformName, const Color& color )
{
	int uniformLocation = GetUniformLocation( uniformName );
	m_glRenderer.SetUniform4f( uniformLocation, color.r, color.g, color.b, color.a );
}


//-----------------------------------------------------------------------------------------------
void Material::BindTextures()
{
	for( unsigned int textureInfoIndex = 0; textureInfoIndex < m_textures.size(); ++textureInfoIndex )
	{
		TextureInfo textureInfo = m_textures[ textureInfoIndex ];
		if( textureInfo.m_texture != nullptr )
		{
			m_glRenderer.SetActiveTexture( textureInfo.m_activeTextureLayer );
			m_glRenderer.BindTexture2D( textureInfo.m_texture->m_openglTextureID );
			m_glRenderer.SetUniform1i( textureInfo.m_uniformLocation, textureInfo.m_activeTextureLayer );
		}
		else
		{
			m_glRenderer.BindTexture2D( 0 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
int Material::GetUniformLocation( const std::string& uniformName )
{
	std::map<std::string, int>::iterator mapIter;
	mapIter = m_uniformLocationsByName.find( uniformName );
	if( mapIter != m_uniformLocationsByName.end() )
		return mapIter->second;

	int uniformLocation = m_glRenderer.GetUniformLocation( m_glShaderProgram->m_programID, uniformName.c_str() );
	m_uniformLocationsByName.insert( std::pair<std::string, int>( uniformName, uniformLocation ) );
	return uniformLocation;
}


//-----------------------------------------------------------------------------------------------
void Material::SendUniformValues()
{
	m_glRenderer.SetModelViewProjectionMatrix( m_glShaderProgram->m_modelViewProjectionMatUniformLocation );
}


//-----------------------------------------------------------------------------------------------
void Material::SendAttribValues()
{
	m_glRenderer.SetPositionAttribPointer( VERTEX_ATTRIB_POSITIONS );
	m_glRenderer.SetColorAttribPointer( VERTEX_ATTRIB_COLORS );
	m_glRenderer.SetTexCoordAttribPointer( VERTEX_ATTRIB_TEX_COORDS );
	m_glRenderer.SetNormalAttribPointer( VERTEX_ATTRIB_NORMALS );
	m_glRenderer.SetTangentAttribPointer( VERTEX_ATTRIB_TANGENTS );
}


//-----------------------------------------------------------------------------------------------
void Material::EnableAttribArrays()
{
	m_glRenderer.EnableAttribArray( VERTEX_ATTRIB_POSITIONS );
	m_glRenderer.EnableAttribArray( VERTEX_ATTRIB_COLORS );
	m_glRenderer.EnableAttribArray( VERTEX_ATTRIB_TEX_COORDS );
	m_glRenderer.EnableAttribArray( VERTEX_ATTRIB_NORMALS );
	m_glRenderer.EnableAttribArray( VERTEX_ATTRIB_TANGENTS );
}