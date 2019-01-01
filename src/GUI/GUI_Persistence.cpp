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
#include <OpenSteer/Annotation.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

bool GUI::showSceneViewPanel                   = false;
bool GUI::showPerformancePanel                 = false;
bool GUI::showRenderSettingsPanel              = false;
bool GUI::showLogPanel                         = false;
bool GUI::showAttributePanel                   = false;
bool GUI::showSymbologyPanel                   = false;
bool GUI::showGUI_Settings_Panel               = false;
bool GUI::showOpenSteerTestPanel               = false;
bool GUI::showOpenSteerPanel                   = false;
bool GUI::showCameraInfoPanel                  = false;
bool GUI::showBingTileSystemPanel              = false;

bool GUI::renderSettingsFillMeshes             = true;
bool GUI::renderSettingsRenderMeshOutlines     = true;
bool GUI::renderSettingsFillPolygons           = true;
bool GUI::renderSettingsRenderPolygonOutlines  = true;
bool GUI::renderSettingsRenderLinearFeatures   = true;
bool GUI::renderSettingsRenderSkyBox           = true;
bool GUI::renderSettingsRenderBingMaps         = true;

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

    setBool(L"showSceneViewPanel",                  showSceneViewPanel);
    setBool(L"showPerformancePanel",                showPerformancePanel);
    setBool(L"showPerformancePanel",                showPerformancePanel);
    setBool(L"showRenderSettingsPanel",             showRenderSettingsPanel);
    setBool(L"showLogPanel",                        showLogPanel);
    setBool(L"showAttributePanel",                  showAttributePanel);
    setBool(L"showGUI_Settings_Panel",              showGUI_Settings_Panel);
    setBool(L"showSymbologyPanel",                  showSymbologyPanel);
    setBool(L"showOpenSteerTestPanel",              showOpenSteerTestPanel);
    setBool(L"showOpenSteerPanel",                  showOpenSteerPanel);
    setBool(L"showCameraInfoPanel",                 showCameraInfoPanel);
    setBool(L"showBingTileSystemPanel",             showBingTileSystemPanel);

    setBool(L"renderSettingsFillMeshes",            renderSettingsFillMeshes);
    setBool(L"renderSettingsRenderMeshOutlines",    renderSettingsRenderMeshOutlines);
    setBool(L"renderSettingsFillPolygons",          renderSettingsFillPolygons);
    setBool(L"renderSettingsRenderPolygonOutlines", renderSettingsRenderPolygonOutlines);
    setBool(L"renderSettingsRenderLinearFeatures",  renderSettingsRenderLinearFeatures);
    setBool(L"renderSettingsRenderSkyBox",          renderSettingsRenderSkyBox);
    setBool(L"renderSettingsRenderBingMaps",        renderSettingsRenderBingMaps);
    setBool(L"OpenSteerAnnotation",                 OpenSteer::enableAnnotation);

    //ColorDistanceShader::getDefaultInstance()->loadState(root);
    ColorSymbology::getInstance("defaultMesh")   ->loadState(root);
    ColorSymbology::getInstance("defaultPolygon")->loadState(root);
    ColorSymbology::getInstance("defaultLinear") ->loadState(root);

    if(root.find(L"cameraEye")    != root.end()) { canvas->getCamera()->setEye   (root[L"cameraEye"]   ->AsVec3()) ;}
    if(root.find(L"cameraCenter") != root.end()) { canvas->getCamera()->setCenter(root[L"cameraCenter"]->AsVec3()) ;}
    if(root.find(L"cameraUp")     != root.end()) { canvas->getCamera()->setUp    (root[L"cameraUp"]    ->AsVec3()) ;}

    if(root.find(L"openSteerCameraDist") != root.end())
    {
        GUI::openSteerCameraDist = root[L"openSteerCameraDist"]->AsNumber();
    }

    canvas->getCamera()->update();

    if(root.find(L"buildingHeightMultiplier") != root.end())
    {
        ColorDistanceShader3D     ::getDefaultInstance()->setHeightMultiplier(root[L"buildingHeightMultiplier"]->AsNumber());
        ColorDistanceDepthShader3D::getDefaultInstance()->setHeightMultiplier(root[L"buildingHeightMultiplier"]->AsNumber());
    }
}

void GUI::saveState()
{
    JSONObject root;

    // Booleans
    root[L"showSceneViewPanel"]                   = new JSONValue(showSceneViewPanel);
    root[L"showPerformancePanel"]                 = new JSONValue(showPerformancePanel);
    root[L"showRenderSettingsPanel"]              = new JSONValue(showRenderSettingsPanel);
    root[L"showLogPanel"]                         = new JSONValue(showLogPanel);
    root[L"showAttributePanel"]                   = new JSONValue(showAttributePanel);
    root[L"showGUI_Settings_Panel"]               = new JSONValue(showGUI_Settings_Panel);
    root[L"showSymbologyPanel"]                   = new JSONValue(showSymbologyPanel);
    root[L"showOpenSteerTestPanel"]               = new JSONValue(showOpenSteerTestPanel);
    root[L"showOpenSteerPanel"]                   = new JSONValue(showOpenSteerPanel);
    root[L"showCameraInfoPanel"]                  = new JSONValue(showCameraInfoPanel);
    root[L"showBingTileSystemPanel"]              = new JSONValue(showBingTileSystemPanel);

    root[L"renderSettingsFillMeshes"]             = new JSONValue(renderSettingsFillMeshes);
    root[L"renderSettingsRenderMeshOutlines"]     = new JSONValue(renderSettingsRenderMeshOutlines);
    root[L"renderSettingsFillPolygons"]           = new JSONValue(renderSettingsFillPolygons);
    root[L"renderSettingsRenderPolygonOutlines"]  = new JSONValue(renderSettingsRenderPolygonOutlines);
    root[L"renderSettingsRenderLinearFeatures"]   = new JSONValue(renderSettingsRenderLinearFeatures);
    root[L"renderSettingsRenderSkyBox"]           = new JSONValue(renderSettingsRenderSkyBox);
    root[L"renderSettingsRenderBingMaps"]         = new JSONValue(renderSettingsRenderBingMaps);
    root[L"buildingHeightMultiplier"]             = new JSONValue(ColorDistanceShader3D::getDefaultInstance()->getHeightMultiplier());

    root[L"cameraEye"]                            = new JSONValue(canvas->getCamera()->getEyeConstRef());
    root[L"cameraCenter"]                         = new JSONValue(canvas->getCamera()->getCenterConstRef());
    root[L"cameraUp"]                             = new JSONValue(canvas->getCamera()->getUpConstRef());

    root[L"OpenSteerAnnotation"]                  = new JSONValue(OpenSteer::enableAnnotation);
    root[L"openSteerCameraDist"]                  = new JSONValue(GUI::openSteerCameraDist);

    ColorSymbology::getInstance("defaultMesh")   ->saveState(root);
    ColorSymbology::getInstance("defaultPolygon")->saveState(root);
    ColorSymbology::getInstance("defaultLinear") ->saveState(root);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}


