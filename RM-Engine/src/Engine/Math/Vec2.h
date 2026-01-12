/**
 * @file Vec2.h
 * @author sumin.park
 * @brief  Math for vector2
 * @version 0.1
 * @date 1/8/2026 2:20:52 PM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm::math {
    struct Vec2 {
        float x, y;

        // ----- Constructors -----
        constexpr Vec2() : x(0), y(0) {}
        constexpr Vec2(float v) : x(v), y(v) {}
        constexpr Vec2(float x, float y) : x(x), y(y) {}

        // ----- Operators -----
        constexpr Vec2 operator+(const Vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }

        constexpr Vec2 operator-(const Vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }

        constexpr Vec2 operator*(float s) const { return {x * s, y * s}; }

        constexpr Vec2 operator/(float s) const { return {x / s, y / s}; }

        Vec2& operator+=(const Vec2& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vec2& operator-=(const Vec2& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        Vec2& operator*=(float s) {
            x *= s;
            y *= s;
            return *this;
        }

        // ----- Length -----
        float LengthSq() const { return x * x + y * y; }
        float Length() const { return std::sqrt(LengthSq()); }

        Vec2 Normalized() const {
            float len = Length();
            return (len > 0.0f) ? (*this / len) : Vec2{};
        }

        void Normalize() {
            float len = Length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
            }
        }

        // ----- 2D-specific helpers -----
        float Angle() const { return std::atan2(y, x); }

        Vec2 Perpendicular() const {
            // 90° CCW
            return {-y, x};
        }

        // ----- Static -----
        static float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

        static float Cross(const Vec2& a, const Vec2& b) {
            // 2D cross product magnitude (z-component)
            return a.x * b.y - a.y * b.x;
        }
    };

    inline constexpr Vec2 operator*(float s, const Vec2& v) {
        return v * s;
    }

}  // namespace rm::math
