#include "player.h"
#include <cmath>

void Player::Update(const PlayerInput &input, float dt)
{
    // Update yaw and pitch based on mouse input
    yaw += input.lookYaw * lookSensitivity;
    pitch += input.lookPitch * lookSensitivity;

    // Clamp pitch to avoid flipping
    if (pitch > 1.5f)
        pitch = 1.5f; // ~85 degrees
    if (pitch < -1.5f)
        pitch = -1.5f; // ~-85 degrees

    // Calculate forward and right vectors based on yaw
    Vec3 forward = {std::sin(yaw), 0, std::cos(yaw)};
    Vec3 right = {std::cos(yaw), 0, -std::sin(yaw)};

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
    position.x += (forward.x * input.moveForward + right.x * input.moveRight) * speed * dt;
    position.z += (forward.z * input.moveForward + right.z * input.moveRight) * speed * dt;
}