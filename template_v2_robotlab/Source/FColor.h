#pragma once

class FColor
{
public:
  FColor()
    :
    r(0.0f),
    g(0.0f),
    b(0.0f)
  {}
  FColor(const FColor& col)
    :
    r(col.r),
    g(col.g),
    b(col.b)
  {}
  FColor(float value)
    :
    r(value),
    g(value),
    b(value)
  {}
  FColor(float red, float green, float blue)
    :
    r(red),
    g(green),
    b(blue)
  {}
  FColor(unsigned char red, unsigned char green, unsigned char blue)
    :
    r((float)red / float(255)),
    g((float)green / float(255)),
    b((float)blue / float(255))
  {}

  FColor& operator=(const FColor& color)
  {
    r = color.r;
    g = color.g;
    b = color.b;
    return *this;
  }

  // multiplication
  FColor operator*(const float& value) const
  {
    FColor res;
    res.r = r * value;
    res.g = g * value;
    res.b = b * value;
    return res;
  }
  FColor& operator*=(const float& value)
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
  FColor operator+(const float& value) const
  {
    FColor res;
    res.r = r + value;
    res.g = g + value;
    res.b = b + value;
    return res;
  }
  FColor& operator+=(const float& value)
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
  FColor operator-(const float& value) const
  {
    FColor res;
    res.r = r - value;
    res.g = g - value;
    res.b = b - value;
    return res;
  }
  FColor& operator-=(const float& value)
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
  FColor operator/(const float& value) const
  {
    FColor res;
    float invValue = 1.0f / value;
    res.r = r * invValue;
    res.g = g * invValue;
    res.b = b * invValue;
    return res;
  }
  FColor& operator/=(const float& value)
  {
    float invValue = 1.0f / value;
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

  bool operator<(const float& rhs) const
  {
    return (r < rhs && g < rhs && b < rhs);
  }

  void Limit(float value)
  {
    r = MIN(r, value);
    b = MIN(b, value);
    g = MIN(g, value);
  }

  void Clamp(float low = 0.0f, float high = 1.0f)
  {
    Limit(high);
    r = MAX(r, low);
    b = MAX(b, low);
    g = MAX(g, low);
  }

  FColor GetClamped(float low = 0.0f, float high = 1.0f) const
  {
    FColor res = *this;
    res.Limit(high);
    res.r = MAX(r, low);
    res.b = MAX(b, low);
    res.g = MAX(g, low);
    return res;
  }

  Pixel To32Bit() const
  {
    Pixel res = 0x000000ff;
    res |= (unsigned char)(r * 255) << 24;
    res |= (unsigned char)(g * 255) << 16;
    res |= (unsigned char)(b * 255) << 8;
    return res;
  }

public:
  float r, g, b;
};

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
