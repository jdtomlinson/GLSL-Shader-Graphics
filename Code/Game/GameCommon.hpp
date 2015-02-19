#ifndef included_GameCommon
#define included_GameCommon
#pragma once

//-----------------------------------------------------------------------------------------------
const float ONE_OVER_SIX = 1.f / 6.f;
const float TWO_OVER_SIX = 2.f / 6.f;
const float FOUR_OVER_SIX = 4.f / 6.f;
const float FIVE_OVER_SIX = 5.f / 6.f;
const float SIXTEEN_BY_NINE = 16.f / 9.f;
const float ONE_OVER_FIFTEEN = 1.f / 15.f;
const float ONE_OVER_SIXTEEN = 1.f / 16.f;
const float ONE_OVER_TWENTYFOUR = ( 1.f / 24.f );
const double FRAME_TIME_SECONDS = ( 1.0 / 60.0 );


//-----------------------------------------------------------------------------------------------
class World;
class DeveloperConsole;
extern World g_world;
extern DeveloperConsole g_developerConsole;


#endif // included_GameCommon