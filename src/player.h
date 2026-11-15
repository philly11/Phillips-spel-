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
    bool jump = false;
    bool jumpHeld = false;
};

class Player
{
public:
    void Update(const PlayerInput &input, float dt, float groundHeight);

    const Vec3 &GetPosition() const { return position; }
    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }

    bool IsOnGround() const { return onGround; }
    float GetVelocityY() const { return velocityY; }

    float GetFeetY() const { return position.y - eyeHeight; }
    void setHorizontal(float x, float z)
    {
        position.x = x;
        position.z = z;
    }

private:
    void ApplyLook(const PlayerInput &input);
    void ApplyMovement(const PlayerInput &input, float dt);
    void HandleJump(const PlayerInput &input, float dt);
    void ApplyGravity(const PlayerInput &input, float dt);
    void ResolveGround(float groundHeight, float dt, bool wasOnGround);

    static constexpr float kMaxPitch = 1.5f;      // about 85 degrees
    static constexpr float kMaxFrameTime = 0.05f; // lag spike guard

    Vec3 position = {0.0f, 1.7f, -6.0f}; // start on open ground, in front of the box
    float yaw = 0.0f;                    // rotation around Y axis
    float pitch = 0.0f;                  // rotation around X axis
    float speed = 5.0f;                  // movement speed in units per second
    float lookSensitivity = 0.003f;      // mouse sensitivity

    float velocityY = 0.0f; // vertical velocity for jumping and gravity
    bool onGround = true;   // is the player on the ground
    float gravity = 20.0f;
    float jumpSpeed = 8.0f;
    float eyeHeight = 1.7f; // height of the player's eyes from the ground

    float fallMultiplier = 1.6f;    // falling is faster than rising
    float lowJumpMultiplier = 2.0f; // extra gravity if you release Space early
    float coyoteTime = 0.1f;        // can still jump this long after leaving a ledge
    float jumpBufferTime = 0.1f;    // a jump pressed this early before landing still counts
    float coyoteTimer = 0.0f;
    float jumpBufferTimer = 0.0f;

    float stepUpSpeed = 8.0f; // speed at which the player steps up onto a box
};