#pragma once
#include "Colors.h"
#include "Math/MathUtilities.h"

class FColor
{
public:
  Float r, g, b;
public:
  FColor()
    :
    r(kZeroF),
    g(kZeroF),
    b(kZeroF)
  {}
  FColor(const FColor& col)
    :
    r(col.r),
    g(col.g),
    b(col.b)
  {}
  FColor(Float value)
    :
    r(value),
    g(value),
    b(value)
  {}
  FColor(Float a_r, Float a_g, Float a_b)
    :
    r(a_r),
    g(a_g),
    b(a_b)
  {}
  FColor(unsigned char a_r, unsigned char a_g, unsigned char a_b)
    :
    r((Float)a_r / Float(255)),
    g((Float)a_g / Float(255)),
    b((Float)a_b / Float(255))
  {}
  FColor(FColor col, unsigned char x)
    :
    r(col.r),
    g(col.g),
    b(col.b)
  {}

  // multiplication
  FColor& operator =(const FColor& color)
  {
    r = color.r;
    g = color.g;
    b = color.b;
    return *this;
  }
  FColor operator*(const Float& value) const
  {
    FColor res;
    res.r = r * value;
    res.g = g * value;
    res.b = b * value;
    return res;
  }
  FColor& operator*=(const Float& value)
  {
    r *= value;
    g *= value;
    b *= value;
    return *this;
  }
  FColor operator*(const FColor& rhs) const
  {
    FColor res;
    res.r = r * rhs.r;
    res.g = g * rhs.g;
    res.b = b * rhs.b;
    return res;
  }
  FColor& operator*=(const FColor& rhs)
  {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    return *this;
  }

  // addition
  FColor operator+(const Float& value) const
  {
    FColor res;
    res.r = r + value;
    res.g = g + value;
    res.b = b + value;
    return res;
  }
  FColor& operator+=(const Float& value)
  {
    r += value;
    g += value;
    b += value;
    return *this;
  }
  FColor operator+(const FColor& rhs) const
  {
    FColor res;
    res.r = r + rhs.r;
    res.g = g + rhs.g;
    res.b = b + rhs.b;
    return res;
  }
  FColor& operator+=(const FColor& rhs)
  {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
  }

  // subtraction
  FColor operator-(const Float& value) const
  {
    FColor res;
    res.r = r - value;
    res.g = g - value;
    res.b = b - value;
    return res;
  }
  FColor& operator-=(const Float& value)
  {
    r -= value;
    g -= value;
    b -= value;
    return *this;
  }
  FColor operator-(const FColor& rhs) const
  {
    FColor res;
    res.r = r - rhs.r;
    res.g = g - rhs.g;
    res.b = b - rhs.b;
    return res;
  }
  FColor& operator-=(const FColor& rhs)
  {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    return *this;
  }

  // division
  FColor operator/(const Float& value) const
  {
    FColor res;
    Float invValue = kOneF / value;
    res.r = r * invValue;
    res.g = g * invValue;
    res.b = b * invValue;
    return res;
  }
  FColor& operator/=(const Float& value)
  {
    Float invValue = kOneF / value;
    r *= invValue;
    g *= invValue;
    b *= invValue;
    return *this;
  }
  FColor operator/(const FColor& rhs) const
  {
    FColor res;
    res.r = r / rhs.r;
    res.g = g / rhs.g;
    res.b = b / rhs.b;
    return res;
  }
  FColor& operator/=(const FColor& rhs)
  {
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    return *this;
  }

  bool operator<(const Float& rhs) const
  {
    return (r < rhs && g < rhs && b < rhs);
  }

  void Limit(Float value)
  {
    r = Min(r, value);
    b = Min(b, value);
    g = Min(g, value);
  }

  void Clamp(Float low = kZeroF, Float high = kOneF)
  {
    Limit(high);
    r = Max(r, low);
    b = Max(b, low);
    g = Max(g, low);
  }

  FColor GetClamped(Float low = kZeroF, Float high = kOneF) const
  {
    FColor res = *this;
    res.Limit(high);
    res.r = Max(r, low);
    res.b = Max(b, low);
    res.g = Max(g, low);
    return res;
  }

  Color To32Bit() const
  {
    Color res;
    res.SetA(255);
    res.SetR((unsigned char)(r * 255));
    res.SetG((unsigned char)(g * 255));
    res.SetB((unsigned char)(b * 255));
    return res;
  }
};

// helper functions
inline FColor Sqrt(const FColor& color)
{
  return FColor(
    Sqrt(color.r),
    Sqrt(color.g),
    Sqrt(color.b));
}

namespace FColors
{
  const static FColor White = { 1.0f, 1.0f, 1.0f };
  const static FColor Black = { 0.0f, 0.0f, 0.0f };
  const static FColor Gray = { 0.3f, 0.3f, 0.3f };
  const static FColor LightGray = { 0.8f, 0.8f, 0.8f };
  const static FColor Red = { 1.0f, 0.0f, 0.0f };
  const static FColor Green = { 0.0f, 1.0f, 0.0f };
  const static FColor Blue = { 0.0f, 0.0f, 1.0f };
  const static FColor Yellow = { 1.0f, 1.0f, 0.0f };
  const static FColor Cyan = { 0.0f, 1.0f, 1.0f };
  const static FColor Magenta = { 1.0f, 0.0f, 1.0f };
}
