# pragma once
# include "ax/SafePointerType.h"
# include <imgui.h>

namespace ax {
namespace NodeEditor {

struct PinId    final: SafePointerType<PinId>    { using SafePointerType::SafePointerType; };
struct NodeId   final: SafePointerType<NodeId>   { using SafePointerType::SafePointerType; };
struct LinkId   final: SafePointerType<LinkId>   { using SafePointerType::SafePointerType; };




} // namespace NodeEditor
} // namespace ax
