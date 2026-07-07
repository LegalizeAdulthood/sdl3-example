#pragma once

#include <sdlcpp/Window.h>

#include <wx/window.h>

namespace wxsdl
{

class SdlCanvas : public wxWindow
{
public:
    explicit SdlCanvas(
        wxWindow *parent,
        wxWindowID id = wxID_ANY,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize,
        long style = wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE);
    ~SdlCanvas() override;

    SdlCanvas(const SdlCanvas &) = delete;
    SdlCanvas &operator=(const SdlCanvas &) = delete;

    [[nodiscard]] sdlcpp::Window &window() noexcept
    {
        return m_window;
    }

    [[nodiscard]] const sdlcpp::Window &window() const noexcept
    {
        return m_window;
    }

private:
    sdlcpp::Window m_window;
};

} // namespace wxsdl
