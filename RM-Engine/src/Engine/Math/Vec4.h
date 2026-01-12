/**
 * @file Vec4.h
 * @author sumin.park
 * @brief  Math for vector4
 * @version 0.1
 * @date 1/8/2026 1:04:21 PM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Vec3.h"

namespace rm::math {
    struct Vec4 {
        float x, y, z, w;

        // ----- Constructors -----
        constexpr Vec4() : x(0), y(0), z(0), w(0) {}
        constexpr Vec4(float v) : x(v), y(v), z(v), w(v) {}
        constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        // From Vec3 + w
        constexpr Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

        // ----- Operators -----
        constexpr Vec4 operator+(const Vec4& rhs) const {
            return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
        }

        constexpr Vec4 operator-(const Vec4& rhs) const {
            return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
        }

        constexpr Vec4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }

        constexpr Vec4 operator/(float s) const { return {x / s, y / s, z / s, w / s}; }

        Vec4& operator+=(const Vec4& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }

        Vec4& operator-=(const Vec4& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }

        Vec4& operator*=(float s) {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        // ----- Length -----
        float LengthSq() const { return x * x + y * y + z * z + w * w; }
        float Length() const { return std::sqrt(LengthSq()); }

        Vec4 Normalized() const {
            float len = Length();
            return (len > 0.0f) ? (*this / len) : Vec4{};
        }

        void Normalize() {
            float len = Length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }
        }

        // ----- Static -----
        static float Dot(const Vec4& a, const Vec4& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }
    };

    inline constexpr Vec4 operator*(float s, const Vec4& v) {
        return v * s;
    }

}  // namespace rm::math