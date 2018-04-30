#include "Transform.h"
#include "Math/MathUtilities.h"

Transform Transform::GetInverse()
{
  return (Transform(mInv, m));
}

bool Transform::IsIdentity()
{
  // get an identity matrix
  Mat44 I = Mat44();
  for (int32 i = 0; i < 4; ++i)
  {
    for (int32 j = 0; j < 4; ++j)
    {
      if (I.m[i][j] != m.m[i][j])
      {
        return false;
      }
    }
  }
  return true;
}

bool Transform::operator==(const Transform & rhs)
{
  for (int32 i = 0; i < 4; ++i)
  {
    for (int32 j = 0; j < 4; ++j)
    {
      if (m.m[i][j] != rhs.m.m[i][j])
      {
        return false;
      }
    }
  }
  return true;
}

bool Transform::operator!=(const Transform & rhs)
{
  return !(*this == rhs);
}

Transform Mul(const Transform & lhs, const Transform & rhs)
{
  Mat44 res = lhs.m * rhs.m;
  Mat44 invRes;
  Inverse(res, &invRes);
  return Transform(res, invRes);
}

Transform Transform::operator*(const Transform & rhs)
{
  return Mul(*this, rhs);
}

Transform Transform::operator*=(const Transform & rhs)
{
  return *this = Mul(*this, rhs);
}

Transform Transform::CreateTranslate(const Float x, const Float y, const Float z)
{
  return CreateTranslate(Vec3(x, y, z));
}

Transform Transform::CreateTranslate(const Vec3 & a_offSet)
{
  Mat44 mat = Mat44(
    1.0f, 0.0f, 0.0f, a_offSet.x,
    0.0f, 1.0f, 0.0f, a_offSet.y,
    0.0f, 0.0f, 1.0f, a_offSet.z,
    0.0f, 0.0f, 0.0f, 1.0f);
  Mat44 matInv = Mat44(
    1.0f, 0.0f, 0.0f, -a_offSet.x,
    0.0f, 1.0f, 0.0f, -a_offSet.y,
    0.0f, 0.0f, 1.0f, -a_offSet.z,
    0.0f, 0.0f, 0.0f, 1.0f);
  return Transform(mat, matInv);
}

Transform Transform::CreateScale(const Float x, const Float y, const Float z)
{
  return CreateScale(Vec3(x, y, z));
}

Transform Transform::CreateScale(const Vec3 & a_scale)
{
  Mat44 mat = Mat44(
    a_scale.x, 0.0f, 0.0f, 0.0f,
    0.0f, a_scale.y, 0.0f, 0.0f,
    0.0f, 0.0f, a_scale.z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
  Mat44 matInv = Mat44(
    1.0f / a_scale.x, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f / a_scale.y, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f / a_scale.z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
  return Transform(mat, matInv);
}

Transform Transform::CreateRotateX(Float degrees)
{
  Float a = Deg2Rad(degrees);
  Float s = Sin(a);
  Float c = Cos(a);

  Mat44 mat = Mat44(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, c, -s, 0.0f,
    0.0f, s, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
  return Transform(mat, Transpose(mat));
}

Transform Transform::CreateRotateY(Float degrees)
{
  Float a = Deg2Rad(degrees);
  Float s = Sin(a);
  Float c = Cos(a);

  Mat44 mat = Mat44(
    c, 0.0f, s, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    -s, 0.0f, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
  return Transform(mat, Transpose(mat));
}

Transform Transform::CreateRotateZ(Float degrees)
{
  Float a = Deg2Rad(degrees);
  Float s = Sin(a);
  Float c = Cos(a);

  Mat44 mat = Mat44(
    c, -s, 0.0f, 0.0f,
    s, c, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
  return Transform(mat, Transpose(mat));
}

Transform Transform::CreateRotateXYZ(const Float x, const Float y, const Float z)
{
  return CreateRotateXYZ(Vec3(x, y, z));
}

Transform Transform::CreateRotateXYZ(const Vec3 & degrees)
{
  return Mul(Mul(CreateRotateX(degrees.x), CreateRotateY(degrees.y)), CreateRotateZ(degrees.z));
}

Transform Transform::CreateAxisAngle(const Vec3 & axis, Float degrees)
{
  Vec3 a = axis.Normalized();
  Float angle = Deg2Rad(degrees);
  Float s = Sin(angle);
  Float c = Cos(angle);
  Mat44 mat = Mat44();

  mat.m[0][0] = a.x * a.x + (1.0f - a.x * a.x) * c;
  mat.m[0][1] = a.x * a.y + (1.0f - c) - a.z  * s;
  mat.m[0][2] = a.x * a.z + (1.0f - c) + a.y  * s;

  mat.m[1][0] = a.x * a.y + (1.0f - c) - a.z  * s;
  mat.m[1][1] = a.y * a.y + (1.0f - a.y * a.y) * c;
  mat.m[1][2] = a.y * a.z + (1.0f - c) + a.x  * s;

  mat.m[2][0] = a.x * a.z + (1.0f - c) - a.z   * s;
  mat.m[2][1] = a.y * a.z + (1.0f - c) + a.y   * s;
  mat.m[2][2] = a.z * a.z + (1.0f - a.z * a.z) * c;

  return Transform(mat, Transpose(mat));
}

Vec3 Transform::TransformPoint(const Vec3 & p)
{
  Float x = p.x, y = p.y, z = p.z;
  Vec3 res;
  res.x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
  res.y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
  res.z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
  Float wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
  if (wp == 1.0f)
  {
    return res;
  }
  return res / wp;
}

void Transform::TransformPoint(const Vec3 & p, Vec3 * pPOut)
{
  Float x = p.x, y = p.y, z = p.z;
  pPOut->x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
  pPOut->y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
  pPOut->z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
  Float wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
  if (wp != 1.0f)
  {
    *pPOut /= wp;
  }
}

Vec3 Transform::TransformVector(const Vec3 & v)
{
  Float x = v.x, y = v.y, z = v.z;
  return Vec3(
    m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
    m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
    m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}

void Transform::TransformVector(const Vec3 & v, Vec3 * pVOut)
{
  Float x = v.x, y = v.y, z = v.z;
  *pVOut = Vec3(
    m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
    m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
    m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}

Vec3 Transform::TransformNormal(const Vec3 & n)
{
  Float x = n.x, y = n.y, z = n.z;
  return Vec3(
    mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z,
    mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z,
    mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z);
}

void Transform::TransformNormal(const Vec3 & n, Vec3 * pNOut)
{
  Float x = n.x, y = n.y, z = n.z;
  (*pNOut).x = mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z;
  (*pNOut).y = mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z;
  (*pNOut).z = mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z;
}

Ray Transform::TransformRay(const Ray & r)
{
  Ray res;
  TransformVector(r.d, &res.d);
  TransformPoint(r.o, &res.o);
  res.t = r.t;
  return res;
}

void Transform::TransformRay(const Ray & r, Ray * pROut)
{
  TransformVector(r.d, &pROut->d);
  TransformPoint(r.o, &pROut->o);
  pROut->t = r.t;
}

AABB Transform::TransformAABB(const AABB & box)
{
  AABB res; // start with empty result
  Vec3 corner;
  for (int i = 0; i < 8; ++i)
  {
    TransformPoint(box.Corner(i), &corner);
    res = res.Union(corner);
  }
  return res;
}

void Transform::TransformAABB(const AABB & box, AABB * pBOut)
{
  AABB b = box; // copy to avoid the case where b and pBOut poInt to the same box
  *pBOut = AABB(); // clear the result
  for (int i = 0; i < 8; ++i)
  {
    *pBOut = pBOut->Union(TransformPoint(b.Corner(i)));
  }
}

bool Transform::SwapsHandedness()
{
  Float det = ((m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])) -
    (m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])) +
    (m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0])));
  return det < 0.0f;
}
