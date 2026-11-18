#include "player.h"

#include <algorithm>
#include <cmath>

namespace
{
// Moves value toward 0 by amount, without overshooting
float MoveTowardZero(float value, float amount)
{
    if (value > 0.0f)
        return std::max(0.0f, value - amount);
    return std::min(0.0f, value + amount);
}
} // namespace

void Player::Update(const PlayerInput &input, float dt, float groundHeight)
{
    const bool wasOnGround = onGround;
    dt = std::min(dt, kMaxFrameTime); // a lag spike must not launch the player

    ApplyLook(input);
    RecoverRecoil(dt);
    ApplyMovement(input, dt);
    HandleJump(input, dt);
    ApplyGravity(input, dt);
    ResolveGround(groundHeight, dt, wasOnGround);
}

void Player::RecoverRecoil(float dt)
{
    recoilPitch = MoveTowardZero(recoilPitch, recoilRecovery * dt);
    recoilYaw = MoveTowardZero(recoilYaw, recoilRecovery * dt);
}

void Player::AddRecoil(float up, float sideways)
{
    recoilPitch -= up; // pitch is positive downward, so up is negative
    recoilYaw += sideways;
}

void Player::ApplyLook(const PlayerInput &input)
{
    yaw += input.lookYaw * lookSensitivity;
    pitch += input.lookPitch * lookSensitivity;
    pitch = std::clamp(pitch, -kMaxPitch, kMaxPitch); // avoid flipping over
}

void Player::ApplyMovement(const PlayerInput &input, float dt)
{
    const Vec3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
    const Vec3 right = {-std::cos(yaw), 0.0f, std::sin(yaw)};

    // Normalize so diagonal movement isn't faster
    float moveForward = input.moveForward;
    float moveRight = input.moveRight;
    const float length = std::sqrt(moveForward * moveForward + moveRight * moveRight);
    if (length > 1.0f)
    {
        moveForward /= length;
        moveRight /= length;
    }

    const float moveSpeed = speed * speedMultiplier;
    position.x += (forward.x * moveForward + right.x * moveRight) * moveSpeed * dt;
    position.z += (forward.z * moveForward + right.z * moveRight) * moveSpeed * dt;
}

void Player::SetEyeHeightMultiplier(float m)
{
    // Move the eyes, not the feet. Otherwise growing would sink the feet
    // into the floor and the ground/wall checks would use the wrong height.
    const float newEyeHeight = kBaseEyeHeight * m;
    position.y += newEyeHeight - eyeHeight;
    eyeHeight = newEyeHeight;
}

void Player::HandleJump(const PlayerInput &input, float dt)
{
    // Buffer: a press just before landing still counts.
    // Coyote: a jump just after leaving a ledge still works.
    jumpBufferTimer = input.jump ? jumpBufferTime : jumpBufferTimer - dt;
    coyoteTimer = onGround ? coyoteTime : coyoteTimer - dt;

    if (jumpBufferTimer > 0.0f && coyoteTimer > 0.0f)
    {
        velocityY = jumpSpeed;
        onGround = false;
        jumpBufferTimer = 0.0f;
        coyoteTimer = 0.0f;
    }
}

void Player::ApplyGravity(const PlayerInput &input, float dt)
{
    float g = gravity;
    if (velocityY < 0.0f)
        g *= fallMultiplier; // falling is faster than rising
    else if (velocityY > 0.0f && !input.jumpHeld)
        g *= lowJumpMultiplier; // released early: shorter hop

    velocityY -= g * dt;
    position.y += velocityY * dt;
}

void Player::ResolveGround(float groundHeight, float dt, bool wasOnGround)
{
    const float groundLevel = groundHeight + eyeHeight;

    if (position.y > groundLevel)
    {
        onGround = false;
        return;
    }

    // Walking up a small step: glide instead of snapping
    if (wasOnGround && velocityY <= 0.0f)
        position.y = std::min(groundLevel, position.y + stepUpSpeed * dt);
    else
        position.y = groundLevel; // landing from a jump or fall

    velocityY = 0.0f;
    onGround = true;
}

Vec3 Player::GetLookDirection() const
{
    const float p = std::clamp(pitch + recoilPitch, -kMaxPitch, kMaxPitch);
    const float y = yaw + recoilYaw;
    return {std::sin(y) * std::cos(p), -std::sin(p), std::cos(y) * std::cos(p)};
}

void Player::TakeDamage(int amount)
{
    if (amount <= 0)
    {
        return;
    }
    health = std::max(0, health - amount);
}

bool Player::IsAlive() const
{
    return health > 0;
}

int Player::GetHealth() const
{
    return health;
}