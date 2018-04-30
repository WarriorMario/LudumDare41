#pragma once
#include "Math/Mat44.h"
#include "Math/Vec3.h"
#include "Math/Vec2.h"
#include "SceneTraversal/Ray.h"
#include "SceneTraversal/AABB.h"

class Transform
{
public:
    Transform() {}
    Transform(const Mat44& mat, const Mat44& matInv)
        :
        m(mat),
        mInv(matInv)
    {}

    Transform GetInverse();

    bool IsIdentity();
    bool operator==(const Transform& rhs);
    bool operator!=(const Transform& rhs);

    static inline friend Transform Mul(const Transform& lhs, const Transform& rhs);
    Transform operator* (const Transform& rhs);
    Transform operator*= (const Transform& rhs);

    static Transform CreateTranslate(const Float x, const Float y, const Float z);
    static Transform CreateTranslate(const Vec3& a_offSet);
    static Transform CreateScale(const Float x, const Float y, const Float z);
    static Transform CreateScale(const Vec3& a_scale);
    static Transform CreateRotateX(Float degrees);
    static Transform CreateRotateY(Float degrees);
    static Transform CreateRotateZ(Float degrees);
    static Transform CreateRotateXYZ(const Float x, const Float y, const Float z);
    static Transform CreateRotateXYZ(const Vec3& degrees);
    static Transform CreateAxisAngle(const Vec3& axis, Float degrees);

    Vec3 TransformPoint(const Vec3& p);
    void TransformPoint(const Vec3& p, Vec3* pPOut);
    Vec3 TransformVector(const Vec3& v);
    void TransformVector(const Vec3& v, Vec3* pVOut);
    Vec3 TransformNormal(const Vec3& n); 
    void TransformNormal(const Vec3& n, Vec3* pNOut);
    Ray TransformRay(const Ray& r);
    void TransformRay(const Ray& r, Ray* pROut);
    AABB TransformAABB(const AABB& box);
    void TransformAABB(const AABB& box, AABB* pBOut);

    bool SwapsHandedness();

    Mat44& GetM()
    {
      return m;
    }
    Mat44& GetMInv()
    {
      return mInv;
    }

private:
    Mat44 m, mInv;
};

