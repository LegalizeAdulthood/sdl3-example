#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuTexture.h>
#include <sdlcpp/Properties.h>
#include <sdlcpp/Window.h>

#include <gtest/gtest.h>

#include <type_traits>

TEST(TestCore, sdlHandlesAreMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<core::sdl::Window>);
    static_assert(!std::is_copy_assignable_v<core::sdl::Window>);
    static_assert(std::is_move_constructible_v<core::sdl::Window>);
    static_assert(std::is_move_assignable_v<core::sdl::Window>);
    ASSERT_FALSE(static_cast<bool>(core::sdl::Window{}));

    static_assert(!std::is_copy_constructible_v<core::sdl::Properties>);
    static_assert(!std::is_copy_assignable_v<core::sdl::Properties>);
    static_assert(std::is_move_constructible_v<core::sdl::Properties>);
    static_assert(std::is_move_assignable_v<core::sdl::Properties>);
    ASSERT_FALSE(static_cast<bool>(core::sdl::Properties{}));

    static_assert(!std::is_copy_constructible_v<core::sdl::GpuTexture>);
    static_assert(!std::is_copy_assignable_v<core::sdl::GpuTexture>);
    static_assert(std::is_move_constructible_v<core::sdl::GpuTexture>);
    static_assert(std::is_move_assignable_v<core::sdl::GpuTexture>);
    ASSERT_FALSE(static_cast<bool>(core::sdl::GpuTexture{}));

    static_assert(!std::is_copy_constructible_v<core::sdl::GpuCommandBuffer>);
    static_assert(!std::is_copy_assignable_v<core::sdl::GpuCommandBuffer>);
    static_assert(std::is_move_constructible_v<core::sdl::GpuCommandBuffer>);
    static_assert(std::is_move_assignable_v<core::sdl::GpuCommandBuffer>);
    ASSERT_FALSE(static_cast<bool>(core::sdl::GpuCommandBuffer{}));
}
