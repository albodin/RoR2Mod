#include "Math.h"

namespace Math {
    Vector2 Vector2::operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 Vector2::operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 Vector2::operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 Vector2::operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return Vector2(x * invScalar, y * invScalar);
    }

    bool Vector2::operator==(const Vector2& other) const {
        return (x == other.x) && (y == other.y);
    }

    bool Vector2::operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    float Vector2::Length() const {
        return std::sqrt(x * x + y * y);
    }

    float Vector2::LengthSquared() const {
        return x * x + y * y;
    }

    Vector2 Vector2::Normalized() const {
        float len = Length();
        if (len < 0.000001f) {
            return Vector2(0, 0);
        }
        float invLen = 1.0f / len;
        return Vector2(x * invLen, y * invLen);
    }

    void Vector2::Normalize() {
        float len = Length();
        if (len < 0.000001f) {
            x = 0;
            y = 0;
            return;
        }
        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
    }

    float Vector2::Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    float Vector2::Distance(const Vector2& other) const {
        return Vector2::Distance(*this, other);
    }

    float Vector2::DistanceSquared(const Vector2& other) const {
        return Vector2::DistanceSquared(*this, other);
    }

    float Vector2::Distance(const Vector2& v1, const Vector2& v2) {
        return std::sqrt(DistanceSquared(v1, v2));
    }

    float Vector2::DistanceSquared(const Vector2& v1, const Vector2& v2) {
        float dx = v2.x - v1.x;
        float dy = v2.y - v1.y;
        return dx * dx + dy * dy;
    }

    Vector3 Vector3::operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 Vector3::operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 Vector3::operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 Vector3::operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return Vector3(x * invScalar, y * invScalar, z * invScalar);
    }

    bool Vector3::operator==(const Vector3& other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    bool Vector3::operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    float Vector3::Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float Vector3::LengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vector3 Vector3::Normalized() const {
        float len = Length();
        if (len < 0.000001f) {
            return Vector3(0, 0, 0);
        }
        float invLen = 1.0f / len;
        return Vector3(x * invLen, y * invLen, z * invLen);
    }

    void Vector3::Normalize() {
        float len = Length();
        if (len < 0.000001f) {
            x = 0;
            y = 0;
            z = 0;
            return;
        }
        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
        z *= invLen;
    }

    float Vector3::Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 Vector3::Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float Vector3::Distance(const Vector3& other) const {
        return Vector3::Distance(*this, other);
    }

    float Vector3::DistanceSquared(const Vector3& other) const {
        return Vector3::DistanceSquared(*this, other);
    }

    float Vector3::Distance(const Vector3& v1, const Vector3& v2) {
        return std::sqrt(DistanceSquared(v1, v2));
    }

    float Vector3::DistanceSquared(const Vector3& v1, const Vector3& v2) {
        float dx = v2.x - v1.x;
        float dy = v2.y - v1.y;
        float dz = v2.z - v1.z;
        return dx * dx + dy * dy + dz * dz;
    }
}