//# pragma once
//# include "imgui.h"
//
//struct ImMatrix
//{
//    float m00, m01, m10, m11, m20, m21;
//
//    ImMatrix() { m00 = m11 = 1.0f; m01 = m10 = m20 = m21 = 0.0f; }
//    ImMatrix(float _m00, float _m01, float _m10, float _m11, float _m20, float _m21) { m00 = _m00; m01 = _m01; m10 = _m10; m11 = _m11; m20 = _m20; m21 = _m21; }
//    // Define constructor and implicit cast operators in imconfig.h to convert back<>forth from your math types and ImMatrix.
//# ifdef IM_MATRIX_CLASS_EXTRA
//    IM_MATRIX_CLASS_EXTRA
//# endif
//
//    IMGUI_API ImMatrix Inverted() const;
//
//    static inline ImMatrix Translation(const ImVec2& p) { return Translation(p.x, p.y); }
//    static inline ImMatrix Translation(float x, float y) { return ImMatrix(1.0f, 0.0f, 0.0f, 1.0f, x, y); }
//
//    static inline ImMatrix Scaling(const ImVec2& p) { return Scaling(p.x, p.y); }
//    static inline ImMatrix Scaling(float x, float y) { return ImMatrix(x, 0.0f, 0.0f, y, 0.0f, 0.0f); }
//
//    static IMGUI_API ImMatrix Rotation(float angle);
//
//    static inline ImMatrix Combine(const ImMatrix& lhs, const ImMatrix& rhs) // lhs * rhs = out
//    {
//        return ImMatrix(
//            rhs.m00 * lhs.m00 + rhs.m10 * lhs.m01,
//            rhs.m01 * lhs.m00 + rhs.m11 * lhs.m01,
//            rhs.m00 * lhs.m10 + rhs.m10 * lhs.m11,
//            rhs.m01 * lhs.m10 + rhs.m11 * lhs.m11,
//            rhs.m00 * lhs.m20 + rhs.m10 * lhs.m21 + rhs.m20,
//            rhs.m01 * lhs.m20 + rhs.m11 * lhs.m21 + rhs.m21);
//    }
//
//    inline void Transform(ImVec2* v, size_t count = 1) const
//    {
//        for (size_t i = 0; i < count; ++i, ++v)
//        {
//            *v = ImVec2(
//                m00 * v->x + m10 * v->y + m20,
//                m01 * v->x + m11 * v->y + m21);
//        }
//    }
//
//    inline ImVec2 Transformed(const ImVec2& v) const
//    {
//        ImVec2 p = v;
//        Transform(&p);
//        return p;
//    }
//};
//
//struct ImDrawListTransformationState
//{
//    unsigned int            VtxStartIdx;
//    ImMatrix                Transformation;
//    float                   LastInvTransformationScale;
//    ImVec2                  LastHalfPixel;
//};
//
//struct ImDrawListTransformation
//{
//    ImDrawList*                             _DrawList;
//    ImVector<ImDrawListTransformationState> _TransformationStack;
//    float                                   _InvTransformationScale;
//    ImVec2                                  _HalfPixel;
//
//    IMGUI_API void ImDrawListTransformation(ImDrawList* drawList);
//    IMGUI_API void Clear();
//    IMGUI_API void SetTransformation(const ImMatrix& transformation);
//    IMGUI_API void ApplyTransformation(const ImMatrix& transformation);
//    IMGUI_API void PopTransformation(int count = 1);
//};