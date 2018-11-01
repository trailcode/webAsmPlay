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
#include <webAsmPlay/GUI.h>

using namespace std;

void GUI::loadState()
{
    ifstream configFile("config.json");
    
    const string config((istreambuf_iterator<char>(configFile)), istreambuf_iterator<char>());

    JSONValue * value = JSON::Parse(config.c_str());

    if(!value) { return ;}

    JSONObject root = value->AsObject();

    auto set = [&root](const wstring & key, bool & value)->void
    {
        if(root.find(key) != root.end()) { value = root[key]->AsBool() ;}
    };

    set(L"showViewMatrixPanel",     showViewMatrixPanel);
    set(L"showMVP_MatrixPanel",     showMVP_MatrixPanel);
    set(L"showSceneViewPanel",      showSceneViewPanel);
    set(L"showPerformancePanel",    showPerformancePanel);
    set(L"showPerformancePanel",    showPerformancePanel);
    set(L"showRenderSettingsPanel", showRenderSettingsPanel);
    set(L"showLogPanel",            showLogPanel);
    set(L"showAttributePanel",      showAttributePanel);
    set(L"showGUI_Settings_Panel",  showGUI_Settings_Panel);
}

void GUI::saveState()
{
    JSONObject root;

    root[L"showViewMatrixPanel"]        = new JSONValue(showViewMatrixPanel);
    root[L"showMVP_MatrixPanel"]        = new JSONValue(showMVP_MatrixPanel);
    root[L"showSceneViewPanel"]         = new JSONValue(showSceneViewPanel);
    root[L"showPerformancePanel"]       = new JSONValue(showPerformancePanel);
    root[L"showRenderSettingsPanel"]    = new JSONValue(showRenderSettingsPanel);
    root[L"showLogPanel"]               = new JSONValue(showLogPanel);
    root[L"showAttributePanel"]         = new JSONValue(showAttributePanel);
    root[L"showGUI_Settings_Panel"] = new JSONValue(showGUI_Settings_Panel);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}


