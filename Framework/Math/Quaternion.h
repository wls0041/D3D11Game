#pragma once

class Quaternion final
{
public:
    Quaternion();
    Quaternion(const float& x, const float& y, const float& z, const float& w);
    ~Quaternion() = default;

public:
    float x;
    float y;
    float z;
    float w;
};