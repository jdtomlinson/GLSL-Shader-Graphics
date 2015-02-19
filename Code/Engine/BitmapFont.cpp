#include "BitmapFont.hpp"


//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( const std::string& glyphSheetFileName, const std::string& xmlMetaDataFileName )
{
	m_glyphSheet = Texture::GetTextureByName( glyphSheetFileName );

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( xmlMetaDataFileName.c_str() );
	if( result.status != pugi::status_ok )
		return;

	pugi::xml_node fontInfoNode = doc.child( "FontDefinition" ).child( "FontInfo" );
	m_fontName = fontInfoNode.attribute( "name" ).value();

	for( pugi::xml_node glyph = fontInfoNode.next_sibling(); glyph != 0; glyph = glyph.next_sibling() )
	{
		unsigned int ucsIndex = glyph.attribute( "ucsIndex" ).as_uint();
		unsigned int sheetNumber = glyph.attribute( "sheet" ).as_uint();
		std::string texCoordMinsString = glyph.attribute( "texCoordMins" ).as_string();
		std::string texCoordMaxsString = glyph.attribute( "texCoordMaxs" ).as_string();
		float ttfA = glyph.attribute( "ttfA" ).as_float();
		float ttfB = glyph.attribute( "ttfB" ).as_float();
		float ttfC = glyph.attribute( "ttfC" ).as_float();

		int minsCommaLocation = texCoordMinsString.find( ',' );
		std::string texCoordMinXString = texCoordMinsString.substr( 0, minsCommaLocation );
		float texCoordMinX = (float) atof( texCoordMinXString.c_str() );
		std::string texCoordMinYString = texCoordMinsString.substr( minsCommaLocation + 1 );
		float texCoordMinY = (float) atof( texCoordMinYString.c_str() );

		int maxsCommaLocation = texCoordMaxsString.find( ',' );
		std::string texCoordMaxXString = texCoordMaxsString.substr( 0, maxsCommaLocation );
		float texCoordMaxX = (float) atof( texCoordMaxXString.c_str() );
		std::string texCoordMaxYString = texCoordMaxsString.substr( maxsCommaLocation + 1 );
		float texCoordMaxY = (float) atof( texCoordMaxYString.c_str() );

		m_glyphData[ ucsIndex ] = Glyph( ucsIndex, sheetNumber, Vector2( texCoordMinX, texCoordMinY ), Vector2( texCoordMaxX, texCoordMaxY ), ttfA, ttfB, ttfC );
	}
}