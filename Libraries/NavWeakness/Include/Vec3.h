/*
Vec3.h
A header file that adds a Vector3 struct with various helper functions.

This file was originally created by github.com/OrfeasZ aka NoFaTe
but has been modified to add one operator and to rename a function.

Licensed under the MIT License
SPDX-License-Identifier: MIT
Copyright (c) 2022+ Anthony Fuller et al.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cmath>

struct Vec3
{
    Vec3() : X(0.f), Y(0.f), Z(0.f) {}
    Vec3(float p_X, float p_Y, float p_Z) : X(p_X), Y(p_Y), Z(p_Z) {}

    float X;
    float Y;
    float Z;

    float DistanceSquaredTo(const Vec3& p_Other) const
    {
        return powf(p_Other.X - X, 2.f) + powf(p_Other.Y - Y, 2.f) + powf(p_Other.Z - Z, 2.f);
    }

    float DistanceTo(const Vec3& p_Other) const
    {
        return sqrtf(DistanceSquaredTo(p_Other));
    }

    Vec3 MidpointTo(const Vec3& p_Other) const
    {
        return Vec3((p_Other.X + X) / 2.f, (p_Other.Y + Y) / 2.f, (p_Other.Z + Z) / 2.f);
    }

    Vec3 operator+(const Vec3& p_Other) const
    {
        return Vec3(X + p_Other.X, Y + p_Other.Y, Z + p_Other.Z);
    }

    Vec3 operator-(const Vec3& p_Other) const
    {
        return Vec3(X - p_Other.X, Y - p_Other.Y, Z - p_Other.Z);
    }

    Vec3 operator*(float p_Value) const
    {
        return Vec3(X * p_Value, Y * p_Value, Z * p_Value);
    }

    Vec3 operator/(float p_Value) const
    {
        return Vec3(X / p_Value, Y / p_Value, Z / p_Value);
    }

    float& operator[](int p_Value)
    {
        return *(&X + p_Value);
    }

    float Dot(const Vec3& p_Other) const
    {
        return X * p_Other.X + Y * p_Other.Y + Z * p_Other.Z;
    }

    Vec3 Cross(const Vec3& p_Other) const
    {
        return Vec3(
            Y * p_Other.Z - Z * p_Other.Y,
            Z * p_Other.X - X * p_Other.Z,
            X * p_Other.Y - Y * p_Other.X);
    }

    float GetMagnitudeSquared() const
    {
        return Dot(*this);
    }

    float GetMagnitude() const
    {
        return sqrtf(GetMagnitudeSquared());
    }

    Vec3 PerpendicularPointTo(Vec3 p_LineStart, Vec3 p_LineEnd) const
    {
        const Vec3 s_LineDirection = (p_LineEnd - p_LineStart).GetUnitVec();
        const Vec3 s_PointToLineStart = (*this - p_LineStart);
        const float s_DistanceFromStartToPerpendicularPoint = s_PointToLineStart.Dot(s_LineDirection);
        const Vec3 s_PerpendicularPoint = p_LineStart + (s_LineDirection * s_DistanceFromStartToPerpendicularPoint);

        return s_PerpendicularPoint;
    }

    Vec3 GetUnitVec() const
    {
        float s_Magnitude = GetMagnitude();

        if (s_Magnitude <= 0.f)
            return Vec3();

        float x = 1.f / s_Magnitude;

        return Vec3(X * x, Y * x, Z * x);
    }
};
