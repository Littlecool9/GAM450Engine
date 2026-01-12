/**
 * @file Vec3.h
 * @author sumin.park
 * @brief  Math for vector3
 * @version 0.1
 * @date 1/8/2026 6:40:49 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm::math
{
    struct Vec3
    {
        float x, y, z;

        // ----- Constructors -----
        constexpr Vec3() : x(0), y(0), z(0) {}
        constexpr Vec3(float v) : x(v), y(v), z(v) {}
        constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        // ----- Operators -----
        constexpr Vec3 operator+(const Vec3& rhs) const {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        constexpr Vec3 operator-(const Vec3& rhs) const {
            return {x - rhs.x, y - rhs.y, z - rhs.z};
        }

        constexpr Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }

        constexpr Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }

        Vec3& operator+=(const Vec3& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vec3& operator-=(const Vec3& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vec3& operator*=(float s) {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        // ----- Length -----
        float LengthSq() const { return x * x + y * y + z * z; }

        float Length() const { return std::sqrt(LengthSq()); }

        Vec3 Normalized() const {
            float len = Length();
            return (len > 0.0f) ? (*this / len) : Vec3{};
        }

        void Normalize() {
            float len = Length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
                z /= len;
            }
        }

        // ----- Static Functions -----
        static float Dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

        static Vec3 Cross(const Vec3& a, const Vec3& b) {
            return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
        }
    };

    // ----- RHS Operations -----
    inline constexpr Vec3 operator*(float s, const Vec3& v) {
        return v * s;
    }

} // rm namespace