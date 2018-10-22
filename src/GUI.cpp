#include <cmath>
//#include <imgui.h>

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
    #define IMGUI_API
    #include <imgui.h>
    #include <imgui_impl_glfw_gl3.h>
    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
    #define IMGUI_IMPL_API
    #include <imgui/imgui.h>
    #include <imgui_impl_opengl3.h>
    #include <imgui_impl_glfw.h>
    
#endif // __EMSCRIPTEN__

#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <iostream>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/simplify/TopologyPreservingSimplifier.h>
#include <geos/simplify/DouglasPeuckerSimplifier.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Renderable.h>
#include <webAsmPlay/Attributes.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tceGeom/vec2.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderableCollection.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/GridPlane.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace geos::geom;
using namespace geos::simplify;
using namespace rsmz;
using namespace glm;
using namespace tce::geom;

///////////////
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
 
 
 //#ifndef IMGUITOOLBAR_H_
 //#define IMGUITOOLBAR_H_
 
 #ifndef IMGUI_API
 #include <imgui.h>
 #endif //IMGUI_API
 
 // SOME EXAMPLE CODE
 /*
 Outside any ImGui::Window:
 
         {
             static ImGui::Toolbar toolbar("myFirstToolbar##foo");
             if (toolbar.getNumButtons()==0)  {
                 char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                 for (int i=0;i<9;i++) {
                     strcpy(tmp,"toolbutton ");
                     sprintf(&tmp[strlen(tmp)],"%d",i+1);
                     uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                     uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);
 
                     toolbar.addButton(ImGui::Toolbutton(tmp,(void*)myImageTextureId2,uv0,uv1));
                 }
                 toolbar.addSeparator(16);
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 11",(void*)myImageTextureId2,uv0,uv1,ImVec2(32,32),true,false,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 12",(void*)myImageTextureId2,uv0,uv1,ImVec2(48,24),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
 
                 toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f));
             }
             const int pressed = toolbar.render();
             if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);
         }
         {
             static ImGui::Toolbar toolbar("myFirstToolbar2##foo");
             if (toolbar.getNumButtons()==0)  {
                 char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                 for (int i=8;i>=0;i--) {
                     strcpy(tmp,"toolbutton ");
                     sprintf(&tmp[strlen(tmp)],"%d",8-i+1);
                     uv0=ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                     uv1=ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);
 
                     toolbar.addButton(ImGui::Toolbutton(tmp,(void*)myImageTextureId2,uv0,uv1,ImVec2(24,48)));
                 }
                 toolbar.addSeparator(16);
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 11",(void*)myImageTextureId2,uv0,uv1,ImVec2(24,32),true,false,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 12",(void*)myImageTextureId2,uv0,uv1,ImVec2(24,32),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
 
                 toolbar.setProperties(true,true,false,ImVec2(0.0f,0.0f),ImVec2(0.25f,0.9f),ImVec4(0.85,0.85,1,1));
 
                 //toolbar.setScaling(2.0f,1.1f);
             }
             const int pressed = toolbar.render();
             if (pressed>=0) fprintf(stderr,"Toolbar2: pressed:%d\n",pressed);
         }
 
 
 Inside a ImGui::Window:
 
         // 2. Show another simple window, this time using an explicit Begin/End pair
         if (show_another_window)
         {
             ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
             {
                 static ImGui::Toolbar toolbar;
                 if (toolbar.getNumButtons()==0)  {
                     char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                     for (int i=0;i<9;i++) {
                         strcpy(tmp,"toolbutton ");
                         sprintf(&tmp[strlen(tmp)],"%d",i+1);
                         uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                         uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);
 
                         toolbar.addButton(ImGui::Toolbutton(tmp,(void*)myImageTextureId2,uv0,uv1,ImVec2(16,16)));
                     }
                     toolbar.addSeparator(16);
                     toolbar.addButton(ImGui::Toolbutton("toolbutton 11",(void*)myImageTextureId2,uv0,uv1,ImVec2(16,16),true,true,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
                     toolbar.addButton(ImGui::Toolbutton("toolbutton 12",(void*)myImageTextureId2,uv0,uv1,ImVec2(16,16),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
 
                     toolbar.setProperties(true,false,false,ImVec2(0.0f,0.f),ImVec2(0.25,1));
                 }
                 const int pressed = toolbar.render();
                 if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);
             }
             // Here we can open a child window if we want to toolbar not to scroll
             ImGui::Text("Hello");
             ImGui::End();
         }
 
 */
 
 namespace ImGui {
 class Toolbar {
 public:
     struct Button {
         char tooltip[1024];
         void* user_texture_id;
         ImVec2 uv0;
         ImVec2 uv1;
         ImVec2 size;
         ImVec4 tint_col;
         bool isToggleButton;
         mutable bool isDown;
         inline Button(const char* _tooltip,void* _user_texture_id,const ImVec2& _uv0,const ImVec2& _uv1,const ImVec2& _size=ImVec2(32,32),bool _isToggleButton=false,bool _isDown=false,const ImVec4& _tint_col=ImVec4(1,1,1,1))
             : user_texture_id(_user_texture_id),uv0(_uv0),uv1(_uv1),size(_size),tint_col(_tint_col),isToggleButton(_isToggleButton),isDown(_isDown) {
             tooltip[0]='\0';
             if (_tooltip) {
                 IM_ASSERT(strlen(_tooltip)<1024);
                 strcpy(tooltip,_tooltip);
             }
         }
         inline Button(const Button& o) {*this=o;}
         inline const Button& operator=(const Button& o) {
             strcpy(tooltip,o.tooltip);
             user_texture_id=o.user_texture_id;
             size=o.size;
             uv0=o.uv0;uv1=o.uv1;tint_col=o.tint_col;
             isToggleButton=o.isToggleButton;
             isDown=o.isDown;
             return *this;
         }
     };
 
 
     Toolbar(const char* _name="",bool _visible=true,bool _keepAButtonSelected=false,bool _vertical=false,bool _lightAllBarWhenHovered=true,const ImVec2& _hvAlignmentsIn01=ImVec2(0.5f,0.f),const ImVec2& _opacityOffAndOn=ImVec2(-1.f,-1.f),const ImVec4& _bg_col=ImVec4(1,1,1,1),const ImVec4& _displayPortion=ImVec4(0,0,-1,-1))
     : visible(_visible),toolbarWindowPos(0,0),toolbarWindowSize(0,0),scaling(1,1),tooltipsDisabled(false)
     {
         IM_ASSERT(_name!=NULL && strlen(_name)<1024);  // _name must be valid
         strcpy(name,_name);
         setProperties(_keepAButtonSelected,_vertical,_lightAllBarWhenHovered,_hvAlignmentsIn01,_opacityOffAndOn,_bg_col,_displayPortion);
     }
     ~Toolbar() {clearButtons();}
     void setProperties(bool _keepAButtonSelected=false,bool _vertical=false,bool _lightAllBarWhenHovered=true,const ImVec2& _hvAlignmentsIn01=ImVec2(0.5f,0.f),const ImVec2& _opacityOffAndOn=ImVec2(-1.f,-1.f),const ImVec4& _bg_col=ImVec4(1,1,1,1),const ImVec4& _displayPortion=ImVec4(0,0,-1,-1)) {
         keepAButtonSelected = _keepAButtonSelected;
         vertical = _vertical;
         hvAlignmentsIn01 = _hvAlignmentsIn01;
         setDisplayProperties(_opacityOffAndOn,_bg_col);
         hoverButtonIndex = selectedButtonIndex = -1;
         lightAllBarWhenHovered=_lightAllBarWhenHovered;
         displayPortion = _displayPortion;
 
         if (buttons.size()>0) updatePositionAndSize();
     }
     void setDisplayProperties(const ImVec2& _opacityOffAndOn=ImVec2(-1.f,-1.f),const ImVec4& _bg_col=ImVec4(1,1,1,1)) {
         opacityOffAndOn = ImVec2(_opacityOffAndOn.x<0 ? 0.35f : _opacityOffAndOn.x,_opacityOffAndOn.y<0 ? 1.f : _opacityOffAndOn.y);
         bgColor = _bg_col;
     }
 
     int render(bool allowdHoveringWhileMouseDragging=false,int limitNumberOfToolButtonsToDisplay=-1) const {
         int pressedItem = -1;
         const int numberToolbuttons = (limitNumberOfToolButtonsToDisplay>=0 && limitNumberOfToolButtonsToDisplay<=buttons.size())?limitNumberOfToolButtonsToDisplay:(int)buttons.size();
         if (numberToolbuttons==0 || !visible) return pressedItem;
         const bool inWindowMode = name[0]=='\0';
         if (!inWindowMode && (toolbarWindowSize.x==0 || toolbarWindowSize.y==0)) updatePositionAndSize();
 
         if (inWindowMode) ImGui::PushID(this);
         // override sizes:
         ImGuiStyle& Style = ImGui::GetStyle();
         const ImVec2 oldWindowPadding = Style.WindowPadding;
         const ImVec2 oldItemSpacing = Style.ItemSpacing;
         if (!inWindowMode) Style.WindowPadding.x=Style.WindowPadding.y=0;
         Style.ItemSpacing.x=Style.ItemSpacing.y=0;              // Hack to fix vertical layout
 
         //
         if (!inWindowMode)  {
             ImGui::SetNextWindowPos(toolbarWindowPos);
             //if (keepAButtonSelected) ImGui::SetNextWindowFocus(); // This line was originally enabled
         }
         else if (keepAButtonSelected && selectedButtonIndex==-1) selectedButtonIndex=0;
         //        
         
         //const float oldWindowBorderSize = Style.WindowBorderSize;Style.WindowBorderSize=0.f;
         //const float oldFrameBorderSize = Style.FrameBorderSize;Style.FrameBorderSize=0.f;

         static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|
                 ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings/*|ImGuiWindowFlags_Tooltip*/;
         const bool dontSkip = inWindowMode || ImGui::Begin(name,NULL,toolbarWindowSize,0,flags);
         if (dontSkip){                        
             ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0,0,0,0));      // Hack not needed in previous ImGui versions: the bg color of the image button should be defined inside ImGui::ImageButton(...). (see below)
             bool noItemHovered = true;float nextSpacingAmount = 0;float currentWidth=Style.WindowPadding.x;const float windowWidth = ImGui::GetWindowWidth();
             float bg_col_opacity = 1.f;ImVec4 bg_col = bgColor;if (bg_col.w>=0) bg_col_opacity = bg_col.w;
             ImVec4 tint_col(1,1,1,1);ImVec2 tbsz(0,0);        
             const bool isMouseDragging = allowdHoveringWhileMouseDragging ? ImGui::IsMouseDragging(0) : false;
             for (int i=0;i<numberToolbuttons;i++)  {
                 const Button& tb = buttons[i];
                 tbsz.x=tb.size.x*scaling.x;tbsz.y=tb.size.y*scaling.y;
                 if (inWindowMode)   {
                     if ((keepAButtonSelected && selectedButtonIndex==i) || (tb.isToggleButton && tb.isDown)) {bg_col.w = 0;tint_col.w = opacityOffAndOn.x;}
                     else {
                         bg_col.w = (i == hoverButtonIndex) ? bg_col_opacity : 0;tint_col = tb.tint_col;
                         tint_col.w = lightAllBarWhenHovered ?  ((hoverButtonIndex>=0) ? opacityOffAndOn.x : opacityOffAndOn.y) : ((i == hoverButtonIndex) ? opacityOffAndOn.x : opacityOffAndOn.y);
                     }
                 }
                 else {
                     if ((keepAButtonSelected && selectedButtonIndex==i) || (tb.isToggleButton && tb.isDown)) {bg_col.w = bg_col_opacity;tint_col.w = opacityOffAndOn.y;}
                     else {
                         bg_col.w = (i == hoverButtonIndex) ? bg_col_opacity : 0;tint_col = tb.tint_col;
                         tint_col.w = lightAllBarWhenHovered ?  ((hoverButtonIndex>=0) ? opacityOffAndOn.y : opacityOffAndOn.x) : ((i == hoverButtonIndex) ? opacityOffAndOn.y : opacityOffAndOn.x);
                     }
                 }
                 if (!vertical)  {
                     if (!tb.user_texture_id) {nextSpacingAmount+=tbsz.x;continue;}
                     else if (i>0) {
                         if (inWindowMode) {
                             currentWidth+=nextSpacingAmount;
                             if (tbsz.x<=windowWidth && currentWidth+tbsz.x>windowWidth) {
                                 currentWidth=Style.WindowPadding.x;
                             }
                             else ImGui::SameLine(0,nextSpacingAmount);
                         }
                         else ImGui::SameLine(0,nextSpacingAmount);
                     }
                     nextSpacingAmount = 0;
                 }
                 else {
                     if (!tb.user_texture_id) {nextSpacingAmount = 0;continue;}
                     else if (i>0) {
                         if (i+1<numberToolbuttons)  {
                             const Button& tbNext = buttons[i+1];
                             if (!tbNext.user_texture_id) nextSpacingAmount+=tbNext.size.y*scaling.y;
                         }
                         ImGui::GetStyle().ItemSpacing.y=nextSpacingAmount;
                     }
                     nextSpacingAmount = 0;
                 }
                 ImGui::PushID(i);
                 if (ImGui::ImageButton(tb.user_texture_id,tbsz,tb.uv0,tb.uv1,0,bg_col,tint_col)) {
                     if (tb.isToggleButton) tb.isDown=!tb.isDown;
                     else if (keepAButtonSelected) selectedButtonIndex = (selectedButtonIndex==i && !inWindowMode) ? -1 : i;
                     pressedItem=i;
                 }
                 ImGui::PopID();
                 if (inWindowMode) currentWidth+=tbsz.x;
                 //const bool isItemHovered = isMouseDragging ? ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly) : ImGui::IsItemHovered();
                 //const bool isItemHovered = isMouseDragging ? ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_RectOnly) : ImGui::IsItemHovered();
                 const bool isItemHovered = isMouseDragging ? false : ImGui::IsItemHovered();
                 if (isItemHovered)
                 {
                     if (!tooltipsDisabled && strlen(tb.tooltip)>0)   {
                         //if (!inWindowMode) ImGui::SetNextWindowFocus();
                         if (!inWindowMode) ToolbarSetTooltip(tb.tooltip,oldWindowPadding);
                         else ImGui::SetTooltip("%s",tb.tooltip);
 
                         /*SetNextWindowPos(ImGui::GetIO().MousePos);
                         ImGui::BeginTooltip();
                         ImGui::Text(tb.tooltip);
                         ImGui::EndTooltip();*/
                     }
                     hoverButtonIndex = i;noItemHovered = false;
                 }
             }
             if (noItemHovered) hoverButtonIndex = -1;
             ImGui::PopStyleColor();                             // Hack not needed in previous ImGui versions (see above)
         }
         if (!inWindowMode) ImGui::End();
         // restore old sizes
         //Style.FrameBorderSize = oldFrameBorderSize;
         //Style.WindowBorderSize = oldWindowBorderSize;

         Style.ItemSpacing = oldItemSpacing;     // Hack to fix vertical layout
         if (!inWindowMode) Style.WindowPadding = oldWindowPadding;
         if (inWindowMode) ImGui::PopID(); // this
 
         return pressedItem;
     }
 
     // called by other methods, but can be called manually after adding new buttons or resizing the screen.
     void updatePositionAndSize() const {
         const int numButtons = (int) buttons.size();
         toolbarWindowSize.x=toolbarWindowSize.y=0;
         if (numButtons==0 || name[0]=='\0') return;
         bool isSeparator;ImVec2 sz(0,0);
         for (int i=0;i<numButtons;i++) {
             const Button& b= buttons[i];
             isSeparator = b.user_texture_id == NULL;
             sz.x=b.size.x*scaling.x;sz.y=b.size.y*scaling.y;
             if (vertical)   {
                 if (!isSeparator && toolbarWindowSize.x < sz.x) toolbarWindowSize.x = sz.x;
                 toolbarWindowSize.y+=sz.y;
             }
             else {
                 if (!isSeparator && toolbarWindowSize.y < sz.y) toolbarWindowSize.y = sz.y;
                 toolbarWindowSize.x+=sz.x;
             }
         }    
         toolbarWindowPos.x = (displayPortion.x<0 ? (-ImGui::GetIO().DisplaySize.x*displayPortion.x) : displayPortion.x)+((displayPortion.z<0 ? (-ImGui::GetIO().DisplaySize.x*displayPortion.z) : displayPortion.z)-toolbarWindowSize.x)*hvAlignmentsIn01.x;
         toolbarWindowPos.y = (displayPortion.y<0 ? (-ImGui::GetIO().DisplaySize.y*displayPortion.y) : displayPortion.y)+((displayPortion.w<0 ? (-ImGui::GetIO().DisplaySize.y*displayPortion.w) : displayPortion.w)-toolbarWindowSize.y)*hvAlignmentsIn01.y;
     }
 
     inline size_t getNumButtons() const {return buttons.size();}
     inline Button* getButton(size_t i) {return ((int)i < buttons.size())? &buttons[i] : NULL;}
     inline const Button* getButton(size_t i) const {return ((int)i < buttons.size())? &buttons[i] : NULL;}
     inline void addButton(const Button& button) {
         buttons.push_back(button);
     }
     inline void addSeparator(float pixels) {
         buttons.push_back(Button("",NULL,ImVec2(0,0),ImVec2(0,0),ImVec2(pixels,pixels)));
     }
     inline void clearButtons() {
         for (int i=0;i<buttons.size();i++) buttons[i].~Button();
         buttons.clear();
     }
 
     inline void setVisible(bool flag) {visible = flag;}
     inline bool getVisible() {return visible;}
 
     inline void setSelectedButtonIndex(int index) const {selectedButtonIndex=index;}
     inline int getSelectedButtonIndex() const {return selectedButtonIndex;}
     inline bool isToggleButtonDown(int index) const {return (index>=0 && index<(int)buttons.size() && buttons[index].isToggleButton) ? buttons[index].isDown : false;}
 
     inline int getHoverButtonIndex() const {return hoverButtonIndex;}
     inline void setScaling(float x,float y) {
         if (scaling.x!=x || scaling.y!=y)   {
             scaling.x=x;scaling.y=y;
             updatePositionAndSize();
         }
     }
     const ImVec2& getScaling() const {return scaling;}
     inline void setDisplayPortion(const ImVec4& _displayPortion) {
         this->displayPortion = _displayPortion;
         updatePositionAndSize();
     }
     const ImVec4& getDisplayPortion() const {return this->displayPortion;}
 
     inline void disableTooltips(bool flag) {tooltipsDisabled=flag;}
 
 
     protected:
     char name[1024];
     bool visible;
 
 #   ifdef _MSC_VER
     public:
 #   endif //_MSC_VER
 
     bool keepAButtonSelected;
 
     mutable int hoverButtonIndex;
     mutable int selectedButtonIndex;
 
     bool vertical;
     ImVec2 hvAlignmentsIn01;
     ImVec2 opacityOffAndOn;
     ImVec4 bgColor;
     bool lightAllBarWhenHovered;
     mutable ImVec4 displayPortion;
 
     ImVector<Button> buttons;
     mutable ImVec2 toolbarWindowPos;
     mutable ImVec2 toolbarWindowSize;
     ImVec2 scaling;
     bool tooltipsDisabled;
 
     friend struct PanelManager;
 
     inline static void ToolbarSetTooltip(const char* tooltip,const ImVec2& windowPadding) {
         ImGuiStyle& igStyle = ImGui::GetStyle();
         igStyle.WindowPadding.x = windowPadding.x;
         igStyle.WindowPadding.y = windowPadding.y;
         ImGui::SetTooltip("%s",tooltip);
         igStyle.WindowPadding.x = igStyle.WindowPadding.y = 0.f;
     }
 
 };
 typedef Toolbar::Button Toolbutton;

 } // namespace Imgui
 
 //#endif //IMGUITOOLBAR_H_
//////////////


#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static ImVec4 clear_color = ImColor(114, 144, 154);
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

GeosTestCanvas  * geosTestCanvas = NULL;
Canvas          * canvas         = NULL;
SkyBox          * skyBox         = NULL;

bool showViewMatrixPanel     = false;
bool showMVP_MatrixPanel     = false;
bool showSceneViewPanel      = false;
bool showPerformancePanel    = false;
bool showRenderSettingsPanel = false;
bool showLogPanel            = false;
bool showAttributePanel      = false;

bool isFirst = true;

FrameBuffer * frameBuffer = NULL;

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void refresh(GLFWwindow* window)
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    glfwMarkWindowForRefresh(window);
#endif
}

int counter = 0;

void cback(char* data, int size, void* arg) {
    std::cout << "Callback " << data << " " << size << std::endl;
    counter++;
}

ImGuiTextBuffer * Buf = NULL;

struct AppLog
{
    //ImGuiTextBuffer     Buf;
    bool                ScrollToBottom;

    void    Clear()     { Buf->clear(); }

    void    AddLog(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        //Buf.appendv(fmt, args);
        //Buf.appendf("%s", "fdasdfasd");
        va_end(args);
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_opened = NULL)
    {
        ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(title, p_opened);
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(Buf->begin());
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
};

AppLog logPanel;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
    /*
#ifdef __EMSCRIPTEN__
    if(Buf) { Buf->append("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
#else
    if(Buf) { Buf->appendf("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
#endif
     */
}

//vector<Renderable *> pickedRenderiables;

//extern void (*debugLoggerFunc)(const std::string & file, const std::string & line, const std::string & message);

GeoClient * client = NULL;

void mainLoop(GLFWwindow * window)
{
    if(!Buf) {  Buf = new ImGuiTextBuffer() ;}
    // Game loop
    
#ifdef __EMSCRIPTEN__
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();

    ImGui_ImplGlfwGL3_NewFrame();
#endif


    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {

                }
                if(ImGui::MenuItem("Test Web Worker"))
                {
                    #ifdef __EMSCRIPTEN__
                        worker_handle worker = emscripten_create_worker("worker.js");
                        emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);
                    #else
                        dmess("Implement me!");
                    #endif
                }

                if(ImGui::MenuItem("Load Geometry"))
                {
                    //GeoClient::getInstance()->loadGeometry(canvas);

                    if(!client) { client = new GeoClient(window) ;}

                    client->loadGeometry(canvas);
                }

                if(ImGui::MenuItem("Load All Geometry"))
                {
                    //GeoClient::getInstance()->loadGeometry(canvas);

                    if(!client) { client = new GeoClient(window) ;}

                    client->loadAllGeometry(canvas);
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("View"))
            {
                if(ImGui::MenuItem("View Matrix"))     { showViewMatrixPanel     = !showViewMatrixPanel     ;}
                if(ImGui::MenuItem("MVP Matrix"))      { showMVP_MatrixPanel     = !showMVP_MatrixPanel     ;}
                if(ImGui::MenuItem("Geos Tests"))      { showSceneViewPanel      = !showSceneViewPanel      ;}
                if(ImGui::MenuItem("Performance"))     { showPerformancePanel    = !showPerformancePanel    ;}
                if(ImGui::MenuItem("Render Settings")) { showRenderSettingsPanel = !showRenderSettingsPanel ;}
                if(ImGui::MenuItem("Log"))             { showLogPanel            = !showLogPanel            ;}
                if(ImGui::MenuItem("Attributes"))      { showAttributePanel      = !showAttributePanel      ;}

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    {
        static uint32_t myImageTextureId2 = 0;
        if(!myImageTextureId2) { myImageTextureId2 = Textures::load("if_Info_131908.png") ;}
        //dmess("myImageTextureId2 " << myImageTextureId2);
             static ImGui::Toolbar toolbar("myFirstToolbar##foo");
             if (toolbar.getNumButtons()==0)  {
                 char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                 for (int i=0;i<9;i++) {
                     strcpy(tmp,"toolbutton ");
                     sprintf(&tmp[strlen(tmp)],"%d",i+1);
                     uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                     uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);
 
                     toolbar.addButton(ImGui::Toolbutton(tmp,(void*)myImageTextureId2,uv0,uv1));
                 }
                 toolbar.addSeparator(16);
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 11",(void*)myImageTextureId2,uv0,uv1,ImVec2(32,32),true,false,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
                 toolbar.addButton(ImGui::Toolbutton("toolbutton 12",(void*)myImageTextureId2,uv0,uv1,ImVec2(48,24),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
 
                 toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f));
             }
             const int pressed = toolbar.render();
             if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);
         }

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    
    static float time = 0.f;
    
    time += ImGui::GetIO().DeltaTime;

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    if(showPerformancePanel)
    {
        ImGui::Begin("Performance", &showPerformancePanel);

            static float f = 0.0f;
            static float frameTimes[100] = {0.f};
            memcpy(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
            frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));

        ImGui::End();
    }

    if(showViewMatrixPanel)
    {
        ImGui::Begin("View Matrix", &showViewMatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getViewRef()).c_str());

        ImGui::End();
    }
    
    if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    if(showMVP_MatrixPanel)
    {
        ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getMVP_Ref()).c_str());

        ImGui::End();
    }

    isFirst = false;

    if(showRenderSettingsPanel)
    {
        ImGui::Begin("Render Settings", &showRenderSettingsPanel);

            static bool fillPolygons            = true;
            static bool renderPolygonOutlines   = true;
            static bool renderSkyBox            = true;

            static bool _fillPolygons           = true;
            static bool _renderPolygonOutlines  = true;
            static bool _renderSkyBox           = true;

            ImGui::Checkbox("Fill Polygons",    &_fillPolygons);
            ImGui::Checkbox("Polygon Outlines", &_renderPolygonOutlines);
            ImGui::Checkbox("SkyBox",           &_renderSkyBox);

            if(fillPolygons != _fillPolygons)
            {
                fillPolygons = _fillPolygons;

                for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

                for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

                Renderable::setDefaultRenderFill(fillPolygons);
            }

            if(renderPolygonOutlines != _renderPolygonOutlines)
            {
                renderPolygonOutlines = _renderPolygonOutlines;

                for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

                for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

                Renderable::setDefaultRenderOutline(renderPolygonOutlines);
            }

            if(renderSkyBox != _renderSkyBox)
            {
                renderSkyBox = _renderSkyBox;

                if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
                else             { canvas->setSkyBox(NULL)   ;}
            }

            static vec4 fillColor(Renderable::getDefaultFillColor());
            static vec4 outlineColor(Renderable::getDefaultOutlineColor());
            
            ImGui::ColorEdit4("Fill", (float*)&fillColor, true);
            ImGui::ColorEdit4("Outline", (float*)&outlineColor, true);

            /*
            static vec4 fillColor(Renderable::getDefaultFillColor());

            ImGui::ColorPicker4("##picker",
                                (float*)&fillColor,
                                //ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                                0);
                                */

        ImGui::End();
    }
    
    if(showLogPanel) { logPanel.Draw("Log", &showLogPanel) ;}

    canvas->render();
    
    string attrsStr;

    if(client)
    {
        vector<pair<Renderable *, Attributes *> > picked = client->pickRenderables(canvas->getCursorPosWC());

        for(pair<Renderable *, Attributes *> & r : picked)
        {
            get<0>(r)->render(canvas->getMVP_Ref());

            attrsStr += r.second->toString();
        }
    }

    if(showAttributePanel)
    {
        ImGui::Begin("Attributes", &showAttributePanel);

            ImGui::Text(attrsStr.c_str());

        ImGui::End();
    }

    geosTestCanvas->setEnabled(showSceneViewPanel);

    if(showSceneViewPanel)
    {
        ImGui::Begin("Geos Tests", &showSceneViewPanel);

            const ImVec2 pos = ImGui::GetCursorScreenPos();

            const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

            geosTestCanvas->setArea(__(pos), __(sceneWindowSize));

            geosTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

            ImGui::GetWindowDrawList()->AddImage(   (void *)geosTestCanvas->render(),
                                                    pos,
                                                    ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                    ImVec2(0, 1),
                                                    ImVec2(1, 0));
            
            static float buffer1 = 0.1;
            static float buffer2 = 0.02;
            static float buffer3 = 0.22;

            ImGui::SliderFloat("buffer1", &buffer1, 0.0f, 0.3f, "buffer1 = %.3f");
            ImGui::SliderFloat("buffer2", &buffer2, 0.0f, 0.3f, "buffer2 = %.3f");
            ImGui::SliderFloat("buffer3", &buffer3, 0.0f, 0.3f, "buffer3 = %.3f");

            geosTestCanvas->setGeomParameters(buffer1, buffer2, buffer3);

        ImGui::End();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Render();

#ifndef __EMSCRIPTEN__

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif
    glfwMakeContextCurrent(window);

    glfwSwapBuffers(window);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return;
    }

    geosTestCanvas->onMouseButton(window, button, action, mods);

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseButton(window, button, action, mods);
    }

    if (mouse_buttons[button] != action) {
      mouse_buttons[button] = action;
      mouse_buttons_down += action == GLFW_PRESS ? 1 : -1;
    }

#ifndef __EMSCRIPTEN__
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#else
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
#endif
    
    refresh(window);
}

Vec2i lastShiftKeyDownMousePos;

void cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    geosTestCanvas->onMousePosition(window, Vec2d(xpos, ypos));

    canvas->onMousePosition(window, Vec2d(xpos, ypos));

    refresh(window);
}

void scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    geosTestCanvas->onMouseScroll(window, Vec2d(xoffset, yoffset));

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseScroll(window, Vec2d(xoffset, yoffset));
    }

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
#else
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    refresh(window);
}

void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    geosTestCanvas->onKey(window, key, scancode, action, mods);

    if(!GImGui->IO.WantCaptureKeyboard) { canvas->onKey(window, key, scancode, action, mods) ;}
 
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
#else
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif

    refresh(window);
}

void charCallback(GLFWwindow * window, unsigned int c)
{
    geosTestCanvas->onChar(window, c);

    canvas->onChar(window, c);

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_CharCallback(window, c);
#else
    ImGui_ImplGlfw_CharCallback(window, c);
#endif
    refresh(window);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Need to use this to get true size because of retina displays.
    glfwGetWindowSize(window, &width, &height);

    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    refresh(window);
}

void windowFocusCallback(GLFWwindow* window, int focused)
{
    if(focused)
    {
        refresh(window);
    }
}

void cursorEnterCallback(GLFWwindow * window, int /* entered */)
{
    refresh(window);
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback1(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE) ;}
}

void initOpenGL(GLFWwindow* window)
{
    debugLoggerFunc = &dmessCallback;

    // Define the viewport dimensions
    static int width, height;
    //glfwGetFramebufferSize(window, &width, &height); 
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    Renderable::ensureShader();
    //GridPlane  ::ensureShader();
    RenderablePolygon::ensureShaders();

    canvas = new Canvas(false);

    dmess("width " << width << " height " << height);

    //canvas->setArea(Vec2i(0,0), Vec2i(width / 2, height / 2));
    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    geosTestCanvas = new GeosTestCanvas();

    skyBox = new SkyBox();

    canvas->setSkyBox(skyBox);

    //GridPlane * gridPlane = new GridPlane();

    //canvas->addRenderiable(gridPlane);
}

void initGeometry()
{
    #ifdef WORKING

    Geometry * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Geometry * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    Geometry * ppp = scopedGeosGeometry(GeosUtil::makeBox(-0.05,-0.6,0.05,0.6));

    Geometry * pppp = scopedGeosGeometry(GeosUtil::makeBox(-0.6,-0.05,0.6,0.05));

    p = scopedGeosGeometry(p->buffer(0.1));
    //*

    p = scopedGeosGeometry(p->difference(pp));

    p = scopedGeosGeometry(p->difference(ppp));

    p = scopedGeosGeometry(p->difference(pppp));
    //*/

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    Renderable * r = Renderable::create(p, trans);
    //Renderable * r = Renderable::create(dynamic_cast<Polygon *>(p)->getExteriorRing());

    r->setFillColor(vec4(0.3,0.3,0,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    //geosTestCanvas->addRenderiable(r);
    canvas->addRenderiable(r);

    /*
    p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,-4,0.4));

    //r = Renderable::create(p, trans);
    r = Renderable::create(p);

    r->setFillColor(vec4(0.3,0.0,0.3,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    canvas->addRenderiable(r);
    //*/

    #endif
}
