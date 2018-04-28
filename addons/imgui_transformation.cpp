//# include "imgui_transformation.h"
//# include <cmath>
//
//// ImMatrix
//ImMatrix ImMatrix::Inverted() const
//{
//    const float d00 = m11;
//    const float d01 = m01;
//
//    const float d10 = m10;
//    const float d11 = m00;
//
//    const float d20 = m10 * m21 - m11 * m20;
//    const float d21 = m00 * m21 - m01 * m20;
//
//    const float d = m00 * d00 - m10 * d01;
//
//    const float invD = d ? 1.0f / d : 0.0f;
//
//    return ImMatrix(
//         d00 * invD, -d01 * invD,
//        -d10 * invD,  d11 * invD,
//         d20 * invD, -d21 * invD);
//}
//
//ImMatrix ImMatrix::Rotation(float angle)
//{
//    const float s = sinf(angle);
//    const float c = cosf(angle);
//
//    return ImMatrix(c, s, -s, c, 0.0f, 0.0f);
//}
//
//
//void ImDrawListTransformation::SetTransformation(const ImMatrix& transformation)
//{
//    if (!_TransformationStack.empty())
//    {
//        ImMatrix finalTransformation;
//
//        for (ImDrawListTransformationState& transient : _TransformationStack)
//            finalTransformation = ImMatrix::Combine(transient.Transformation, finalTransformation);
//        finalTransformation = finalTransformation.Inverted();
//        finalTransformation = ImMatrix::Combine(transformation, finalTransformation);
//        ApplyTransformation(finalTransformation);
//    }
//    else
//        ApplyTransformation(transformation);
//}
//
//void ImDrawListTransformation::ApplyTransformation(const ImMatrix& transformation)
//{
//    ImDrawListTransformationState tr;
//    tr.Transformation             = transformation;
//    tr.VtxStartIdx                = _VtxCurrentIdx;
//    tr.LastInvTransformationScale = _InvTransformationScale;
//    tr.LastHalfPixel              = _HalfPixel;
//    _TransformationStack.push_back(tr);
//
//    const float scaleX = sqrtf(
//        transformation.m00 * transformation.m00 +
//        transformation.m01 * transformation.m01);
//
//    const float scaleY = sqrtf(
//        transformation.m10 * transformation.m10 +
//        transformation.m11 * transformation.m11);
//
//    const float scale = (scaleX + scaleY) * 0.5f;
//
//    const float invScale = scale > 0.0f ? (1.0f / scale) : 1.0f;
//
//    _InvTransformationScale = _InvTransformationScale * invScale;
//    _HalfPixel              = _HalfPixel              * invScale;
//}
//
//void ImDrawListTransformation::PopTransformation(int count)
//{
//    IM_ASSERT(_TransformationStack.Size > 0);
//
//    for (int i = 0; i < count; ++i)
//    {
//        const ImDrawListTransformationState& tr = _TransformationStack.back();
//        if (tr.VtxStartIdx < _VtxCurrentIdx)
//        {
//            const ImMatrix& m = tr.Transformation;
//
//            ImDrawVert* const vertexBegin = VtxBuffer.Data + tr.VtxStartIdx;
//            ImDrawVert* const vertexEnd   = VtxBuffer.Data + _VtxCurrentIdx;
//
//            for (ImDrawVert* vertex = vertexBegin; vertex != vertexEnd; ++vertex)
//            {
//                const float x = vertex->pos.x;
//                const float y = vertex->pos.y;
//
//                vertex->pos.x = m.m00 * x + m.m10 * y + m.m20;
//                vertex->pos.y = m.m01 * x + m.m11 * y + m.m21;
//            }
//        }
//
//        _InvTransformationScale = tr.LastInvTransformationScale;
//        _HalfPixel              = tr.LastHalfPixel;
//
//        _TransformationStack.pop_back();
//    }
//}
