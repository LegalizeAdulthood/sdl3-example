#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>

#include <sdlcpp/Library.h>
#include <wxsdl/SdlCanvas.h>

#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>

namespace
{

class MandelFrame : public wxFrame
{
public:
    MandelFrame();
};

class MandelApp : public wxApp
{
public:
    MandelApp();
    bool OnInit() override;

private:
    sdlcpp::Library m_sdl{SDL_INIT_VIDEO};
};

MandelApp::MandelApp()
{
    SDL_SetHint(SDL_HINT_WINDOWS_ENABLE_MENU_MNEMONICS, "1");
    SDL_SetEventEnabled(SDL_EVENT_KEY_DOWN, false);
    SDL_SetEventEnabled(SDL_EVENT_KEY_UP, false);
    SDL_SetEventEnabled(SDL_EVENT_TEXT_EDITING, false);
    SDL_SetEventEnabled(SDL_EVENT_TEXT_INPUT, false);
    SDL_SetEventEnabled(SDL_EVENT_TEXT_EDITING_CANDIDATES, false);
    SDL_SetEventEnabled(SDL_EVENT_MOUSE_MOTION, false);
    SDL_SetEventEnabled(SDL_EVENT_MOUSE_BUTTON_DOWN, false);
    SDL_SetEventEnabled(SDL_EVENT_MOUSE_BUTTON_UP, false);
    SDL_SetEventEnabled(SDL_EVENT_MOUSE_WHEEL, false);
}

MandelFrame::MandelFrame() :
    wxFrame(nullptr, wxID_ANY, "SDL3 Mandel", wxDefaultPosition, wxSize(800, 600))
{
    auto *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "&Quit");

    auto *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    wxFrameBase::SetMenuBar(menuBar);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, wxID_EXIT);

    auto *sizer = new wxBoxSizer(wxVERTICAL);
    auto *canvas = new wxsdl::SdlCanvas(this);

    sizer->Add(canvas, 1, wxEXPAND);
    SetSizer(sizer);
    wxTopLevelWindowBase::Layout();
}

bool MandelApp::OnInit()
{
    if (!wxApp::OnInit())
    {
        return false;
    }

    auto *frame = new MandelFrame();
    frame->Show();
    return true;
}

} // namespace

wxIMPLEMENT_APP(MandelApp);
