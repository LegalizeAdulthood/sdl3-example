#include "MandelRenderHost.h"

#include <sdlcpp/Library.h>
#include <sdlcpp/sdl.h>
#include <wxsdl/SdlCanvas.h>

#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/window.h>

#include <memory>

namespace
{

constexpr int CpuPresentationMenuId = wxID_HIGHEST + 1;
constexpr int GpuPresentationMenuId = wxID_HIGHEST + 2;

class MandelFrame : public wxFrame
{
public:
    MandelFrame();

private:
    wxWindow *m_cpuDisplay{};
    wxsdl::SdlCanvas *m_canvas{};
    std::unique_ptr<mandel::MandelRenderHost> m_renderHost;
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
    sdlcpp::SetHint(SDL_HINT_WINDOWS_ENABLE_MENU_MNEMONICS, "1");
    sdlcpp::SetEventEnabled(SDL_EVENT_KEY_DOWN, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_KEY_UP, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_TEXT_EDITING, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_TEXT_INPUT, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_TEXT_EDITING_CANDIDATES, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_MOUSE_MOTION, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_MOUSE_BUTTON_DOWN, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_MOUSE_BUTTON_UP, false);
    sdlcpp::SetEventEnabled(SDL_EVENT_MOUSE_WHEEL, false);
}

MandelFrame::MandelFrame() :
    wxFrame(nullptr, wxID_ANY, "SDL3 Mandel", wxDefaultPosition, wxSize(800, 600))
{
    auto *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "&Quit");

    auto *viewMenu = new wxMenu;
    viewMenu->AppendRadioItem(CpuPresentationMenuId, "&CPU");
    viewMenu->AppendRadioItem(GpuPresentationMenuId, "&GPU");
    viewMenu->Check(CpuPresentationMenuId, true);

    auto *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(viewMenu, "&View");
    wxFrameBase::SetMenuBar(menuBar);

    m_cpuDisplay =
        new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE);
    m_canvas = new wxsdl::SdlCanvas(this);
    m_renderHost = std::make_unique<mandel::MandelRenderHost>(*this, *m_cpuDisplay, *m_canvas);

    Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, wxID_EXIT);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) { m_renderHost->SelectCpuPresentation(); }, CpuPresentationMenuId);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) { m_renderHost->SelectGpuPresentation(); }, GpuPresentationMenuId);
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
