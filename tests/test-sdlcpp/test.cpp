#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuDevice.h>
#include <sdlcpp/GpuTexture.h>
#include <sdlcpp/GpuWindowClaim.h>
#include <sdlcpp/Properties.h>
#include <sdlcpp/Window.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

namespace
{

template <typename T>
class SdlHandleTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(SdlHandleTest);

TYPED_TEST_P(SdlHandleTest, sdlHandlesAreMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<TypeParam>);
    static_assert(!std::is_copy_assignable_v<TypeParam>);
    static_assert(std::is_move_constructible_v<TypeParam>);
    static_assert(std::is_move_assignable_v<TypeParam>);
    ASSERT_FALSE(static_cast<bool>(TypeParam{}));
}

REGISTER_TYPED_TEST_SUITE_P(SdlHandleTest, sdlHandlesAreMoveOnly);

class SdlHandleNames
{
public:
    template <typename T>
    static std::string GetName(int)
    {
        if constexpr (std::is_same_v<T, sdlcpp::Window>)
        {
            return "Window";
        }
        else if constexpr (std::is_same_v<T, sdlcpp::Properties>)
        {
            return "Properties";
        }
        else if constexpr (std::is_same_v<T, sdlcpp::GpuTexture>)
        {
            return "GpuTexture";
        }
        else if constexpr (std::is_same_v<T, sdlcpp::GpuCommandBuffer>)
        {
            return "GpuCommandBuffer";
        }
        else if constexpr (std::is_same_v<T, sdlcpp::GpuDevice>)
        {
            return "GpuDevice";
        }
        else
        {
            static_assert(std::is_same_v<T, sdlcpp::GpuWindowClaim>);
            return "GpuWindowClaim";
        }
    }
};

using SdlHandleTypes = ::testing::Types<sdlcpp::Window, sdlcpp::Properties, sdlcpp::GpuTexture,
    sdlcpp::GpuCommandBuffer, sdlcpp::GpuDevice, sdlcpp::GpuWindowClaim>;

INSTANTIATE_TYPED_TEST_SUITE_P(SdlHandles, SdlHandleTest, SdlHandleTypes, SdlHandleNames);

} // namespace
