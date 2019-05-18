/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

#include <streambuf>
#include <fstream>
#include <locale>
#include <codecvt>
#include <JSON.h>
//#include <nlohmann/json.hpp>
#include <OpenSteer/Annotation.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;
//using namespace nlohmann;

bool GUI::s_showSceneViewPanel                   = false;
bool GUI::s_showPerformancePanel                 = false;
bool GUI::s_showRenderSettingsPanel              = false;
bool GUI::s_showLogPanel                         = false;
bool GUI::s_showAttributePanel                   = false;
bool GUI::s_showSymbologyPanel                   = false;
bool GUI::s_showGUI_Settings_Panel               = false;
bool GUI::s_showOpenSteerTestPanel               = false;
bool GUI::s_showOpenSteerPanel                   = false;
bool GUI::s_showCameraInfoPanel                  = false;
bool GUI::s_showBingTileSystemPanel              = false;
bool GUI::s_showFrameBufferDepthDebugPanel		 = false;
bool GUI::s_showBingMapsFrameBufferDebugPanel	 = false;
bool GUI::s_showNormalFrameBufferDebugPanel		 = false;

bool GUI::s_renderSettingsFillMeshes             = true;
bool GUI::s_renderSettingsRenderMeshOutlines     = true;
bool GUI::s_renderSettingsFillPolygons           = true;
bool GUI::s_renderSettingsRenderPolygonOutlines  = true;
bool GUI::s_renderSettingsRenderLinearFeatures   = true;
bool GUI::s_renderSettingsRenderSkyBox           = true;
bool GUI::s_renderSettingsRenderBingMaps         = true;

float GUI::s_lineWidthRender = 1.0;

void GUI::loadState()
{
    dmess("GUI::loadState");

    ifstream configFile("config.json");
    
    const string config((istreambuf_iterator<char>(configFile)), istreambuf_iterator<char>());

    JSONValue * value = JSON::Parse(config.c_str());

    if(!value) { return ;}

    JSONObject root = value->AsObject();

    // Booleans
    auto setBool = [&root](const wstring & key, bool & value)->void
    {
        if(root.find(key) != root.end()) { value = root[key]->AsBool() ;}
    };

    auto setNumber = [&root](const wstring & key, auto & value)->void
    {
        if(root.find(key) != root.end()) { value = root[key]->AsNumber() ;}
    };

    setBool(L"showSceneViewPanel",                  s_showSceneViewPanel);
    setBool(L"showPerformancePanel",                s_showPerformancePanel);
    setBool(L"showPerformancePanel",                s_showPerformancePanel);
    setBool(L"showRenderSettingsPanel",             s_showRenderSettingsPanel);
    setBool(L"showLogPanel",                        s_showLogPanel);
    setBool(L"showAttributePanel",                  s_showAttributePanel);
    setBool(L"showGUI_Settings_Panel",              s_showGUI_Settings_Panel);
    setBool(L"showSymbologyPanel",                  s_showSymbologyPanel);
    setBool(L"showOpenSteerTestPanel",              s_showOpenSteerTestPanel);
    setBool(L"showOpenSteerPanel",                  s_showOpenSteerPanel);
    setBool(L"showCameraInfoPanel",                 s_showCameraInfoPanel);
    setBool(L"showBingTileSystemPanel",             s_showBingTileSystemPanel);
	setBool(L"showFrameBufferDepthDebugPanel",		s_showFrameBufferDepthDebugPanel);
	setBool(L"showBingMapsFrameBufferDebugPanel",	s_showBingMapsFrameBufferDebugPanel);
	setBool(L"showNormalFrameBufferDebugPanel",		s_showNormalFrameBufferDebugPanel);

    setBool(L"renderSettingsFillMeshes",            s_renderSettingsFillMeshes);
    setBool(L"renderSettingsRenderMeshOutlines",    s_renderSettingsRenderMeshOutlines);
    setBool(L"renderSettingsFillPolygons",          s_renderSettingsFillPolygons);
    setBool(L"renderSettingsRenderPolygonOutlines", s_renderSettingsRenderPolygonOutlines);
    setBool(L"renderSettingsRenderLinearFeatures",  s_renderSettingsRenderLinearFeatures);
    setBool(L"renderSettingsRenderSkyBox",          s_renderSettingsRenderSkyBox);
    setBool(L"renderSettingsRenderBingMaps",        s_renderSettingsRenderBingMaps);
    setBool(L"OpenSteerAnnotation",                 OpenSteer::enableAnnotation);

    setNumber(L"cameraMode",                        s_cameraMode);
    setNumber(L"openSteerCameraDist",               openSteerCameraDist);

    ColorSymbology::getInstance("defaultMesh")   ->loadState(root);
    ColorSymbology::getInstance("defaultPolygon")->loadState(root);
    ColorSymbology::getInstance("defaultLinear") ->loadState(root);

    if(root.find(L"cameraEye")    != root.end()) { s_canvas->getCamera()->setEye   (root[L"cameraEye"]   ->AsVec3()) ;}
    if(root.find(L"cameraCenter") != root.end()) { s_canvas->getCamera()->setCenter(root[L"cameraCenter"]->AsVec3()) ;}
    if(root.find(L"cameraUp")     != root.end()) { s_canvas->getCamera()->setUp    (root[L"cameraUp"]    ->AsVec3()) ;}

    s_canvas->getCamera()->update();

    if(root.find(L"buildingHeightMultiplier") != root.end())
    {
        ColorDistanceDepthShader3D::getDefaultInstance()->setHeightMultiplier(root[L"buildingHeightMultiplier"]->AsNumber());
    }
}

void GUI::saveState()
{
    JSONObject root;

    // Booleans
    root[L"showSceneViewPanel"]                   = new JSONValue(s_showSceneViewPanel);
    root[L"showPerformancePanel"]                 = new JSONValue(s_showPerformancePanel);
    root[L"showRenderSettingsPanel"]              = new JSONValue(s_showRenderSettingsPanel);
    root[L"showLogPanel"]                         = new JSONValue(s_showLogPanel);
    root[L"showAttributePanel"]                   = new JSONValue(s_showAttributePanel);
    root[L"showGUI_Settings_Panel"]               = new JSONValue(s_showGUI_Settings_Panel);
    root[L"showSymbologyPanel"]                   = new JSONValue(s_showSymbologyPanel);
    root[L"showOpenSteerTestPanel"]               = new JSONValue(s_showOpenSteerTestPanel);
    root[L"showOpenSteerPanel"]                   = new JSONValue(s_showOpenSteerPanel);
    root[L"showCameraInfoPanel"]                  = new JSONValue(s_showCameraInfoPanel);
    root[L"showBingTileSystemPanel"]              = new JSONValue(s_showBingTileSystemPanel);
	root[L"showFrameBufferDepthDebugPanel"]       = new JSONValue(s_showFrameBufferDepthDebugPanel);
	root[L"showBingMapsFrameBufferDebugPanel"]    = new JSONValue(s_showBingMapsFrameBufferDebugPanel);
	root[L"showNormalFrameBufferDebugPanel"]      = new JSONValue(s_showNormalFrameBufferDebugPanel);

    root[L"renderSettingsFillMeshes"]             = new JSONValue(s_renderSettingsFillMeshes);
    root[L"renderSettingsRenderMeshOutlines"]     = new JSONValue(s_renderSettingsRenderMeshOutlines);
    root[L"renderSettingsFillPolygons"]           = new JSONValue(s_renderSettingsFillPolygons);
    root[L"renderSettingsRenderPolygonOutlines"]  = new JSONValue(s_renderSettingsRenderPolygonOutlines);
    root[L"renderSettingsRenderLinearFeatures"]   = new JSONValue(s_renderSettingsRenderLinearFeatures);
    root[L"renderSettingsRenderSkyBox"]           = new JSONValue(s_renderSettingsRenderSkyBox);
    root[L"renderSettingsRenderBingMaps"]         = new JSONValue(s_renderSettingsRenderBingMaps);
    
    root[L"cameraEye"]                            = new JSONValue(s_canvas->getCamera()->getEyeConstRef());
    root[L"cameraCenter"]                         = new JSONValue(s_canvas->getCamera()->getCenterConstRef());
    root[L"cameraUp"]                             = new JSONValue(s_canvas->getCamera()->getUpConstRef());

    root[L"OpenSteerAnnotation"]                  = new JSONValue(OpenSteer::enableAnnotation);
    root[L"openSteerCameraDist"]                  = new JSONValue(openSteerCameraDist);

    root[L"cameraMode"]                           = new JSONValue(s_cameraMode);

    ColorSymbology::getInstance("defaultMesh")   ->saveState(root);
    ColorSymbology::getInstance("defaultPolygon")->saveState(root);
    ColorSymbology::getInstance("defaultLinear") ->saveState(root);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}


