#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuTexture.h>
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
        if constexpr (std::is_same_v<T, core::sdl::Window>)
        {
            return "Window";
        }
        else if constexpr (std::is_same_v<T, core::sdl::Properties>)
        {
            return "Properties";
        }
        else if constexpr (std::is_same_v<T, core::sdl::GpuTexture>)
        {
            return "GpuTexture";
        }
        else
        {
            static_assert(std::is_same_v<T, core::sdl::GpuCommandBuffer>);
            return "GpuCommandBuffer";
        }
    }
};

using SdlHandleTypes = ::testing::Types<
    core::sdl::Window,
    core::sdl::Properties,
    core::sdl::GpuTexture,
    core::sdl::GpuCommandBuffer>;

INSTANTIATE_TYPED_TEST_SUITE_P(
    SdlHandles,
    SdlHandleTest,
    SdlHandleTypes,
    SdlHandleNames);

} // namespace
