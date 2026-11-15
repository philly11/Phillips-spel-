#pragma once

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct PlayerInput
{
    float moveForward = 0.0f; // -1 to 1
    float moveRight = 0.0f;   // -1 to 1
    float lookYaw = 0.0f;     // mouse delta this frame
    float lookPitch = 0.0f;
};

class Player
{
public:
    void Update(const PlayerInput &input, float dt);

    const Vec3 &GetPosition() const { return position; }
    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }

private:
    Vec3 position = {0.0f, 1.7f, 0.0f}; // starting position
    float yaw = 0.0f;                   // rotation around Y axis
    float pitch = 0.0f;                 // rotation around X axis
    float speed = 5.0f;                 // movement speed in units per second
    float lookSensitivity = 0.003f;     // mouse sensitivity
};