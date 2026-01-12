/**
 * @file Mat4.h
 * @author dream
 * @brief Math for matrix 4x4
 * @version 0.1
 * @date 1/8/2026 1:24:28 PM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

#include "Vec4.h"

namespace rm::math {
    struct Mat4 {
        // Column-major: m[col][row]
        float m[4][4];

        // ----- Constructors -----
        constexpr Mat4() : m{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {}

        constexpr explicit Mat4(float diag)
            : m{{diag, 0, 0, 0}, {0, diag, 0, 0}, {0, 0, diag, 0}, {0, 0, 0, diag}} {}

        static constexpr Mat4 Identity() { return Mat4(1.0f); }

        // ----- Access -----
        float* operator[](int col) { return m[col]; }
        const float* operator[](int col) const { return m[col]; }

        // ----- Matrix * Vector -----
        Vec4 operator*(const Vec4& v) const {
            return {m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
                    m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
                    m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
                    m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w};
        }

        // ----- Matrix * Matrix -----
        Mat4 operator*(const Mat4& rhs) const {
            Mat4 result(0.0f);

            for (int c = 0; c < 4; ++c) {
                for (int r = 0; r < 4; ++r) {
                    for (int k = 0; k < 4; ++k) {
                        result.m[c][r] += m[k][r] * rhs.m[c][k];
                    }
                }
            }
            return result;
        }

        // ----- Transform Builders -----
        static Mat4 Translate(const Vec3& t) {
            Mat4 r = Identity();
            r.m[3][0] = t.x;
            r.m[3][1] = t.y;
            r.m[3][2] = t.z;
            return r;
        }

        static Mat4 Scale(const Vec3& s) {
            Mat4 r(1.0f);
            r.m[0][0] = s.x;
            r.m[1][1] = s.y;
            r.m[2][2] = s.z;
            return r;
        }

        static Mat4 Scale(float s) { return Scale(Vec3{s}); }
    };

}  // namespace rm::math