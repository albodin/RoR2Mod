#pragma once
#include <cmath>

namespace Math {
class Vector2 {
  public:
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}

    operator bool() const { return x != 0.0f || y != 0.0f; }
    bool operator!() const { return x == 0.0f && y == 0.0f; }

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    float Length() const;
    float LengthSquared() const;
    Vector2 Normalized() const;
    void Normalize();
    float Dot(const Vector2& other) const;

    float Distance(const Vector2& other) const;
    float DistanceSquared(const Vector2& other) const;

    static float Distance(const Vector2& v1, const Vector2& v2);
    static float DistanceSquared(const Vector2& v1, const Vector2& v2);
};

class Vector3 {
  public:
    float x;
    float y;
    float z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    operator bool() const { return x != 0.0f || y != 0.0f || z != 0.0f; }
    bool operator!() const { return x == 0.0f && y == 0.0f && z == 0.0f; }

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;

    float Length() const;
    float LengthSquared() const;
    Vector3 Normalized() const;
    void Normalize();
    float Dot(const Vector3& other) const;
    Vector3 Cross(const Vector3& other) const;

    float Distance(const Vector3& other) const;
    float DistanceSquared(const Vector3& other) const;

    static float Distance(const Vector3& v1, const Vector3& v2);
    static float DistanceSquared(const Vector3& v1, const Vector3& v2);
};

struct alignas(16) Matrix4x4 {
    union {
        float m[4][4];
        float m16[16];
        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };

    Matrix4x4() : m00(1), m01(0), m02(0), m03(0), m10(0), m11(1), m12(0), m13(0), m20(0), m21(0), m22(1), m23(0), m30(0), m31(0), m32(0), m33(1) {}
};
} // namespace Math

using Vector2 = Math::Vector2;
using Vector3 = Math::Vector3;
using Matrix4x4 = Math::Matrix4x4;
