#include <doctest/doctest.h>
#include <cmath>
#include "player.h"

static const float kDt = 1.0f / 60.0f; // 60 FPS

static float Distance(const Vec3 &a, const Vec3 &b)
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) +
                     (a.z - b.z) * (a.z - b.z));
}

TEST_CASE("standing still on ")
{
    Player player;
    const Vec3 start = player.GetPosition();
    for (int i = 0; i < 60; ++i)
        player.Update(PlayerInput{}, kDt, 0.0f);

    CHECK(player.GetPosition().y == doctest::Approx(start.y));
    CHECK(player.IsOnGround());
}

TEST_CASE("forward input moves along +Z at yaw 0")
{
    Player player;
    const Vec3 start = player.GetPosition();
    PlayerInput input;
    input.moveForward = 1.0f;
    player.Update(input, kDt, 0.0f);
    CHECK(player.GetPosition().z > start.z);
    CHECK(player.GetPosition().x == doctest::Approx(start.x));
}

TEST_CASE("diagonal movement is not faster than straight")
{
    Player straight, diagonal;
    const Vec3 s0 = straight.GetPosition();
    const Vec3 d0 = diagonal.GetPosition();

    PlayerInput a;
    a.moveForward = 1.0f;
    PlayerInput b;
    b.moveForward = 1.0f;
    b.moveRight = 1.0f;

    straight.Update(a, kDt, 0.0f);
    diagonal.Update(b, kDt, 0.0f);

    CHECK(Distance(s0, straight.GetPosition()) ==
          doctest::Approx(Distance(d0, diagonal.GetPosition())));
}

TEST_CASE("jumping leaves ground and land")
{
    Player player;
    PlayerInput jump;
    jump.jump = true;
    jump.jumpHeld = true;
    player.Update(jump, kDt, 0.0f);

    REQUIRE_FALSE(player.IsOnGround());
    CHECK(player.GetVelocityY() > 0.0f);

    for (int i = 0; i < 180; ++i)
        player.Update(PlayerInput{}, kDt, 0.0f);

    CHECK(player.IsOnGround());
    CHECK(player.GetPosition().y == doctest::Approx(1.7f));
}