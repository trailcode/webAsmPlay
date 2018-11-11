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
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

bool GUI::showViewMatrixPanel      = false;
bool GUI::showMVP_MatrixPanel      = false;
bool GUI::showSceneViewPanel       = false;
bool GUI::showPerformancePanel     = false;
bool GUI::showRenderSettingsPanel  = false;
bool GUI::showLogPanel             = false;
bool GUI::showAttributePanel       = false;
bool GUI::showSymbologyPanel       = false;
bool GUI::showGUI_Settings_Panel   = false;
bool GUI::showOpenSteerPanel       = false;

bool GUI::renderSettingsFillPolygons           = true;
bool GUI::renderSettingsRenderPolygonOutlines  = true;
bool GUI::renderSettingsRenderLinearFeatures   = true;
bool GUI::renderSettingsRenderSkyBox           = true;

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

    setBool(L"showViewMatrixPanel",     showViewMatrixPanel);
    setBool(L"showMVP_MatrixPanel",     showMVP_MatrixPanel);
    setBool(L"showSceneViewPanel",      showSceneViewPanel);
    setBool(L"showPerformancePanel",    showPerformancePanel);
    setBool(L"showPerformancePanel",    showPerformancePanel);
    setBool(L"showRenderSettingsPanel", showRenderSettingsPanel);
    setBool(L"showLogPanel",            showLogPanel);
    setBool(L"showAttributePanel",      showAttributePanel);
    setBool(L"showGUI_Settings_Panel",  showGUI_Settings_Panel);
    setBool(L"showSymbologyPanel",      showSymbologyPanel);
    setBool(L"showOpenSteerPanel",      showOpenSteerPanel);

    setBool(L"renderSettingsFillPolygons",          renderSettingsFillPolygons);
    setBool(L"renderSettingsRenderPolygonOutlines", renderSettingsRenderPolygonOutlines);
    setBool(L"renderSettingsRenderLinearFeatures",  renderSettingsRenderLinearFeatures);
    setBool(L"renderSettingsRenderSkyBox",          renderSettingsRenderSkyBox);

    // Floats

    typedef float (*_setFloat)(const float & value);

    ColorDistanceShader * shader = ColorDistanceShader::getDefaultInstance();

    auto setFloat = [&root, shader](const wstring & key, _setFloat setFunc)->void
    {
        //if(root.find(key) != root.end()) { shader->setFunc(root[key]->AsNumber()) ;}
    };

    //setFloat(L"ColorDistanceShader::minDist", &ColorDistanceShader::setMinDist);
    //setFloat(L"ColorDistanceShader::maxDist", &ColorDistanceShader::setMaxDist);

    typedef vec4 (*_setVec4)(const vec4 & value);

    auto setVec4 = [&root](const wstring & key, _setVec4 setFunc)->void
    {
        if(root.find(key) != root.end()) { setFunc(root[key]->AsVec4()) ;}
    };

    //setVec4(L"ColorDistanceShader::minColor", &ColorDistanceShader::setMinColor);
    //setVec4(L"ColorDistanceShader::maxColor", &ColorDistanceShader::setMaxColor);

    ColorDistanceShader  ::getDefaultInstance()->loadState(root);
    ColorDistanceShader3D::getDefaultInstance()->loadState(root);
}

void GUI::saveState()
{
    JSONObject root;

    // Booleans
    root[L"showViewMatrixPanel"]      = new JSONValue(showViewMatrixPanel);
    root[L"showMVP_MatrixPanel"]      = new JSONValue(showMVP_MatrixPanel);
    root[L"showSceneViewPanel"]       = new JSONValue(showSceneViewPanel);
    root[L"showPerformancePanel"]     = new JSONValue(showPerformancePanel);
    root[L"showRenderSettingsPanel"]  = new JSONValue(showRenderSettingsPanel);
    root[L"showLogPanel"]             = new JSONValue(showLogPanel);
    root[L"showAttributePanel"]       = new JSONValue(showAttributePanel);
    root[L"showGUI_Settings_Panel"]   = new JSONValue(showGUI_Settings_Panel);
    root[L"showSymbologyPanel"]       = new JSONValue(showSymbologyPanel);
    root[L"showOpenSteerPanel"]       = new JSONValue(showOpenSteerPanel);

    root[L"renderSettingsFillPolygons"]           = new JSONValue(renderSettingsFillPolygons);
    root[L"renderSettingsRenderPolygonOutlines"]  = new JSONValue(renderSettingsRenderPolygonOutlines);
    root[L"renderSettingsRenderLinearFeatures"]   = new JSONValue(renderSettingsRenderLinearFeatures);
    root[L"renderSettingsRenderSkyBox"]           = new JSONValue(renderSettingsRenderSkyBox);

    ColorDistanceShader  ::getDefaultInstance()->saveState(root);
    ColorDistanceShader3D::getDefaultInstance()->saveState(root);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}


