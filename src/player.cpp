#include "player.h"
#include <algorithm>
#include <cmath>

void Player::Update(const PlayerInput &input, float dt, float groundHeight)
{
    // Lag spike
    dt = std::min(dt, 0.05f); // Clamp dt to avoid large jumps in position

    // Update yaw and pitch based on mouse input
    yaw += input.lookYaw * lookSensitivity;
    pitch += input.lookPitch * lookSensitivity;
    pitch = std::clamp(pitch, -1.5f, 1.5f); // ~85 degrees

    // Clamp pitch to avoid flipping
    /* if (pitch > 1.5f)
        pitch = 1.5f; // ~85 degrees
    if (pitch < -1.5f)
        pitch = -1.5f; // ~-85 degrees */

    // Calculate forward and right vectors based on yaw
    Vec3 forward = {std::sin(yaw), 0, std::cos(yaw)};
    Vec3 right = {-std::cos(yaw), 0, std::sin(yaw)};

    // Normalize the movement input
    float moveForward = input.moveForward;
    float moveRight = input.moveRight;
    float length = std::sqrt(moveForward * moveForward + moveRight * moveRight);
    if (length > 1.0f)
    {
        moveForward /= length;
        moveRight /= length;
    }

    // Update position based on movement input and speed
    position.x += (forward.x * moveForward + right.x * moveRight) * speed * dt;
    position.z += (forward.z * moveForward + right.z * moveRight) * speed * dt;

    // jump buffer and coyote time logic
    jumpBufferTimer = input.jump ? jumpBufferTime : jumpBufferTimer - dt;
    coyoteTimer = onGround ? coyoteTime : coyoteTimer - dt;

    if (jumpBufferTimer > 0.0f && coyoteTimer > 0.0f)
    {
        velocityY = jumpSpeed;
        onGround = false;
        jumpBufferTimer = 0.0f;
        coyoteTimer = 0.0f;
    }

    float prevFeetY = position.y - eyeHeight;

    // gravity, faster when falling
    float g = gravity;
    if (velocityY < 0.0f)
    {
        g *= fallMultiplier;
    }
    else if (velocityY > 0.0f && !input.jumpHeld)
    {
        g *= lowJumpMultiplier;
    }

    velocityY -= g * dt;
    position.y += velocityY * dt;
    float feetY = position.y - eyeHeight;

    float groundLevel = groundHeight + eyeHeight;
    if (position.y <= groundLevel)
    {
        position.y = groundLevel;
        velocityY = 0.0f;
        onGround = true;
    }
    else
    {
        onGround = false;
    }
}