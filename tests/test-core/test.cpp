#include <core/core.h>
#include <core/sdl/GpuCommandBuffer.h>
#include <core/sdl/GpuTexture.h>
#include <core/sdl/Properties.h>
#include <core/sdl/Window.h>

#include <gtest/gtest.h>

#include <type_traits>

TEST(TestCore, basic)
{
    ASSERT_TRUE(true);
}

TEST(TestCore, sdl_handles_are_move_only)
{
    static_assert(!std::is_copy_constructible_v<core::sdl::Window>);
    static_assert(!std::is_copy_assignable_v<core::sdl::Window>);
    static_assert(std::is_move_constructible_v<core::sdl::Window>);
    static_assert(std::is_move_assignable_v<core::sdl::Window>);

    static_assert(!std::is_copy_constructible_v<core::sdl::Properties>);
    static_assert(!std::is_copy_assignable_v<core::sdl::Properties>);
    static_assert(std::is_move_constructible_v<core::sdl::Properties>);
    static_assert(std::is_move_assignable_v<core::sdl::Properties>);

    static_assert(!std::is_copy_constructible_v<core::sdl::GpuTexture>);
    static_assert(!std::is_copy_assignable_v<core::sdl::GpuTexture>);
    static_assert(std::is_move_constructible_v<core::sdl::GpuTexture>);
    static_assert(std::is_move_assignable_v<core::sdl::GpuTexture>);

    static_assert(!std::is_copy_constructible_v<core::sdl::GpuCommandBuffer>);
    static_assert(!std::is_copy_assignable_v<core::sdl::GpuCommandBuffer>);
    static_assert(std::is_move_constructible_v<core::sdl::GpuCommandBuffer>);
    static_assert(std::is_move_assignable_v<core::sdl::GpuCommandBuffer>);

    ASSERT_FALSE(static_cast<bool>(core::sdl::Window{}));
    ASSERT_FALSE(static_cast<bool>(core::sdl::Properties{}));
    ASSERT_FALSE(static_cast<bool>(core::sdl::GpuTexture{}));
    ASSERT_FALSE(static_cast<bool>(core::sdl::GpuCommandBuffer{}));
}
