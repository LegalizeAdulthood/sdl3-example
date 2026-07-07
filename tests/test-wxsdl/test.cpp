#include <wxsdl/SdlCanvas.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(WxSdlCanvas, sdlCanvasIsWxWindow)
{
    static_assert(std::is_base_of_v<wxWindow, wxsdl::SdlCanvas>);
    static_assert(!std::is_copy_constructible_v<wxsdl::SdlCanvas>);
    static_assert(!std::is_copy_assignable_v<wxsdl::SdlCanvas>);
    static_assert(std::is_destructible_v<wxsdl::SdlCanvas>);

    SUCCEED();
}

} // namespace
