/**
 * @file Mat3.h
 * @author sumin.park
 * @brief  Math for matrix 3x3
 * @version 0.1
 * @date 1/8/2026 1:39:44 PM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Mat4.h"
#include "Vec4.h"
#include "Vec3.h"

namespace rm::math {
    struct Mat3 {
        // Column-major: m[col][row]
        float m[3][3];

        // ----- Constructors -----
        constexpr Mat3() : m{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} {}

        constexpr explicit Mat3(float diag) : m{{diag, 0, 0}, {0, diag, 0}, {0, 0, diag}} {}
        
        constexpr Mat3(float c0r0, float c0r1, float c0r2,
                       float c1r0, float c1r1, float c1r2,
                       float c2r0, float c2r1, float c2r2)
            : m{{c0r0, c0r1, c0r2}, {c1r0, c1r1, c1r2}, {c2r0, c2r1, c2r2}} {}

        static constexpr Mat3 Identity() { return Mat3(1.0f); }

        // ----- Access -----
        float* operator[](int col) { return m[col]; }
        const float* operator[](int col) const { return m[col]; }

        // ----- Matrix * Vector -----
        Vec3 operator*(const Vec3& v) const {
            return {m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
                    m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
                    m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z};
        }

        // ----- Matrix * Matrix -----
        Mat3 operator*(const Mat3& rhs) const {
            Mat3 result(0.0f);

            for (int c = 0; c < 3; ++c) {
                for (int r = 0; r < 3; ++r) {
                    for (int k = 0; k < 3; ++k) {
                        result.m[c][r] += m[k][r] * rhs.m[c][k];
                    }
                }
            }
            return result;
        }

        // ----- Transpose -----
        Mat3 Transposed() const {
            Mat3 r(0.0f);
            for (int c = 0; c < 3; ++c)
                for (int r0 = 0; r0 < 3; ++r0)
                    r.m[r0][c] = m[c][r0];
            return r;
        }

        // ----- Determinant -----
        float Determinant() const {
            return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                   m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
                   m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
        }

        // ----- Inverse -----
        Mat3 Inversed() const {
            float det = Determinant();
            if (det == 0.0f)
                return Mat3(0.0f);

            float invDet = 1.0f / det;
            Mat3 r(0.0f);

            r.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
            r.m[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * invDet;
            r.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;

            r.m[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * invDet;
            r.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
            r.m[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * invDet;

            r.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
            r.m[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * invDet;
            r.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;

            return r;
        }

        // ----- Rotation (Axis-Angle) -----
        static Mat3 Rotation(const Vec3& axis, float radians) {
            Vec3 a = axis.Normalized();
            float c = std::cos(radians);
            float s = std::sin(radians);
            float t = 1.0f - c;

            Mat3 r(1.0f);

            r.m[0][0] = t * a.x * a.x + c;
            r.m[0][1] = t * a.x * a.y + s * a.z;
            r.m[0][2] = t * a.x * a.z - s * a.y;

            r.m[1][0] = t * a.x * a.y - s * a.z;
            r.m[1][1] = t * a.y * a.y + c;
            r.m[1][2] = t * a.y * a.z + s * a.x;

            r.m[2][0] = t * a.x * a.z + s * a.y;
            r.m[2][1] = t * a.y * a.z - s * a.x;
            r.m[2][2] = t * a.z * a.z + c;

            return r;
        }

        // ----- 2D Rotation (Z-axis) -----
        static Mat3 RotationZ(float radians) {
            float c = std::cos(radians);
            float s = std::sin(radians);

            Mat3 r(1.0f);
            r.m[0][0] = c;
            r.m[1][0] = -s;
            r.m[0][1] = s;
            r.m[1][1] = c;
            return r;
        }
    };

}  // namespace rm::math