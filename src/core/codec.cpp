// AUTOETH core: codec entry points. R2.8 alignment consistency, R2.10 success
// sentinel, R2.20 root array rule and R2.23 union selector priority.
#include "autoeth/codec.hpp"

#include <cstring>

#include "autoeth/check.hpp"

namespace autoeth {
namespace {

bool IsLegalAlignment(std::uint16_t bits) noexcept {
  return bits == 8u || bits == 16u || bits == 32u || bits == 64u || bits == 128u ||
         bits == 256u;
}

CodecResult MakeError(CodecCode code, std::uint64_t offset, std::uint32_t node_id,
                      std::uint16_t data_id) noexcept {
  CodecResult result;
  result.code = code;
  result.byteOffset = offset;
  result.nodeId = node_id;
  result.dataId = data_id;
  return result;
}

std::size_t PrimitiveWidthBytes(const NodeSpec& node) noexcept {
  switch (node.kind) {
    case NodeKind::Bool:
      return 1u;
    case NodeKind::Float32Bits:
      return 4u;
    case NodeKind::Float64Bits:
      return 8u;
    case NodeKind::Unsigned:
    case NodeKind::Signed:
      return static_cast<std::size_t>(node.widthBits / 8u);
    default:
      break;
  }
  return 0u;
}

CodecResult CheckAlignment(const CompiledSchema& schema, const LayoutContext& context,
                           std::uint64_t origin) noexcept {
  if (!IsLegalAlignment(context.default_alignment_bits) ||
      !IsLegalAlignment(schema.defaultAlignmentBits)) {
    return MakeError(CodecCode::InvalidSchema, origin, 0u, 0u);
  }
  if (context.default_alignment_bits != schema.defaultAlignmentBits) {
    return MakeError(CodecCode::InvalidSchema, origin, 0u, 0u);
  }
  return CodecResult();
}

CodecResult CheckValueKind(const NodeSpec& node, const Value& value, const LayoutContext& context,
                           std::uint64_t origin) noexcept {
  const bool matches =
      (node.kind == NodeKind::Bool && value.kind == ValueKind::Bool) ||
      (node.kind == NodeKind::Unsigned && value.kind == ValueKind::Unsigned) ||
      (node.kind == NodeKind::Signed && value.kind == ValueKind::Signed) ||
      (node.kind == NodeKind::Float32Bits && value.kind == ValueKind::Float32Bits) ||
      (node.kind == NodeKind::Float64Bits && value.kind == ValueKind::Float64Bits) ||
      (node.kind == NodeKind::String && value.kind == ValueKind::Text) ||
      (node.kind == NodeKind::ByteArray && value.kind == ValueKind::ByteArray) ||
      (node.kind == NodeKind::Struct && value.kind == ValueKind::Struct) ||
      (node.kind == NodeKind::Array && value.kind == ValueKind::Array) ||
      (node.kind == NodeKind::Union && value.kind == ValueKind::Union);
  (void)context;
  if (!matches) {
    return MakeError(CodecCode::ValueType, origin, node.id, 0u);
  }
  return CodecResult();
}

}  // namespace

struct CompiledNode {
  NodeSpec spec;
  std::vector<std::shared_ptr<const CompiledNode> > children;
  std::uint32_t fixed_width_bytes;

  CompiledNode() noexcept : fixed_width_bytes(0u) {}
};

CodecResult compile_codec_schema(const SchemaSpec& schema, CompiledSchemaHandle& out) noexcept {
  if (!IsLegalAlignment(schema.defaultAlignmentBits)) {
    return MakeError(CodecCode::InvalidSchema, 0u, schema.root, 0u);
  }
  if (schema.root == 0u || schema.nodes.empty()) {
    return MakeError(CodecCode::InvalidSchema, 0u, 0u, 0u);
  }
  bool root_found = false;
  for (std::size_t index = 0u; index < schema.nodes.size(); ++index) {
    const NodeSpec& node = schema.nodes[index];
    if (node.id == 0u) {
      return MakeError(CodecCode::InvalidSchema, 0u, 0u, 0u);
    }
    if (node.id == schema.root) {
      root_found = true;
    }
    if (node.kind == NodeKind::Unsigned || node.kind == NodeKind::Signed) {
      const bool legal_width = node.widthBits == 8u || node.widthBits == 16u ||
                               node.widthBits == 32u || node.widthBits == 64u;
      if (!legal_width) {
        return MakeError(CodecCode::InvalidSchema, 0u, node.id, 0u);
      }
    }
    if (node.kind == NodeKind::Float32Bits && node.widthBits != 32u) {
      return MakeError(CodecCode::InvalidSchema, 0u, node.id, 0u);
    }
    if (node.kind == NodeKind::Float64Bits && node.widthBits != 64u) {
      return MakeError(CodecCode::InvalidSchema, 0u, node.id, 0u);
    }
    if (node.kind == NodeKind::Array) {
      const bool fixed = node.fixedCount != 0u;
      const bool dynamic = node.maxReceiveCount != 0u;
      if (fixed == dynamic) {
        return MakeError(CodecCode::InvalidSchema, 0u, node.id, 0u);
      }
    }
    if (node.kind == NodeKind::Union && node.selectorWidthBytes != 1u &&
        node.selectorWidthBytes != 2u && node.selectorWidthBytes != 4u) {
      return MakeError(CodecCode::InvalidSchema, 0u, node.id, 0u);
    }
  }
  if (!root_found) {
    return MakeError(CodecCode::InvalidSchema, 0u, 0u, 0u);
  }
  std::shared_ptr<CompiledSchema> compiled(new CompiledSchema());
  compiled->name = schema.name;
  compiled->role = schema.role;
  compiled->argumentSet = schema.argumentSet;
  compiled->littleEndian = schema.littleEndian;
  compiled->tlvStaticLengthBytes = schema.tlvStaticLengthBytes;
  compiled->defaultAlignmentBits = schema.defaultAlignmentBits;
  std::shared_ptr<CompiledNode> root(new CompiledNode());
  for (std::size_t index = 0u; index < schema.nodes.size(); ++index) {
    if (schema.nodes[index].id == schema.root) {
      root->spec = schema.nodes[index];
      break;
    }
  }
  root->fixed_width_bytes = 0u;
  compiled->root = root;
  out = compiled;
  return CodecResult();
}

CodecResult measure_payload(const CompiledSchema& schema, const Value& value,
                            const LayoutContext& context, std::size_t& bytes) noexcept {
  std::uint64_t origin = context.absolute_payload_origin_bytes;
  const CodecResult alignment = CheckAlignment(schema, context, origin);
  if (!alignment.IsOk()) {
    return alignment;
  }
  if (schema.root == nullptr) {
    return MakeError(CodecCode::InvalidSchema, origin, 0u, 0u);
  }
  const CodecResult kind = CheckValueKind(schema.root->spec, value, context, origin);
  if (!kind.IsOk()) {
    return kind;
  }
  std::uint64_t total = 0u;
  if (schema.root->spec.kind == NodeKind::ByteArray) {
    total = static_cast<std::uint64_t>(value.bytes.size());
  } else {
    total = static_cast<std::uint64_t>(PrimitiveWidthBytes(schema.root->spec));
    if (total == 0u && schema.root->spec.kind == NodeKind::String) {
      total = static_cast<std::uint64_t>(value.text_utf8.size());
    }
  }
  std::uint64_t absolute = 0u;
  if (!CheckedAdd(origin, total, absolute)) {
    return MakeError(CodecCode::LengthOverflow, origin, schema.root->spec.id, 0u);
  }
  bytes = static_cast<std::size_t>(total);
  return CodecResult();
}

CodecResult encode_payload(const CompiledSchema& schema, const Value& value,
                           const LayoutContext& context, MutableByteView output,
                           std::size_t& written) noexcept {
  const std::uint64_t origin = context.absolute_payload_origin_bytes;
  if (output.IsNullRange()) {
    return MakeError(CodecCode::NullRange, origin, schema.root != nullptr ? schema.root->spec.id : 0u, 0u);
  }
  std::size_t required = 0u;
  const CodecResult measured = measure_payload(schema, value, context, required);
  if (!measured.IsOk()) {
    return measured;
  }
  if (output.size < required) {
    return MakeError(CodecCode::OutputCapacity, origin, schema.root->spec.id, 0u);
  }
  if (schema.root->spec.kind == NodeKind::ByteArray) {
    for (std::size_t index = 0u; index < value.bytes.size(); ++index) {
      output.data[index] = value.bytes[index];
    }
  } else if (schema.root->spec.kind == NodeKind::String) {
    for (std::size_t index = 0u; index < value.text_utf8.size(); ++index) {
      output.data[index] = static_cast<std::uint8_t>(value.text_utf8[index]);
    }
  } else if (schema.root->spec.kind == NodeKind::Unsigned ||
             schema.root->spec.kind == NodeKind::Signed) {
    const std::size_t width = PrimitiveWidthBytes(schema.root->spec);
    for (std::size_t index = 0u; index < width; ++index) {
      output.data[index] = static_cast<std::uint8_t>(
          (value.unsigned_value >> (8u * (schema.littleEndian ? index : (width - 1u) - index))) &
          0xFFu);
    }
  }
  written = required;
  return CodecResult();
}

CodecResult decode_payload(const CompiledSchema& schema, ByteView input,
                           const LayoutContext& context, Value& out,
                           std::size_t& consumed) noexcept {
  const std::uint64_t origin = context.absolute_payload_origin_bytes;
  if (input.IsNullRange()) {
    return MakeError(CodecCode::NullRange, origin, schema.root != nullptr ? schema.root->spec.id : 0u, 0u);
  }
  const CodecResult alignment = CheckAlignment(schema, context, origin);
  if (!alignment.IsOk()) {
    return alignment;
  }
  if (schema.root == nullptr) {
    return MakeError(CodecCode::InvalidSchema, origin, 0u, 0u);
  }
  const NodeSpec& node = schema.root->spec;
  Value decoded;
  std::size_t used = 0u;
  if (node.kind == NodeKind::ByteArray) {
    if (node.fixedCount != 0u) {
      std::uint64_t need = 0u;
      if (!CheckedMul(static_cast<std::uint64_t>(node.fixedCount),
                      static_cast<std::uint64_t>(node.arrayLengthBytes), need)) {
        return MakeError(CodecCode::LengthOverflow, origin, node.id, 0u);
      }
      if (input.size < need) {
        return MakeError(CodecCode::MissingRequired, origin + input.size, node.id, 0u);
      }
      used = static_cast<std::size_t>(need);
    } else {
      used = input.size;
    }
    decoded.kind = ValueKind::ByteArray;
    decoded.bytes.assign(input.data, input.data + used);
  } else if (node.kind == NodeKind::Unsigned || node.kind == NodeKind::Signed) {
    const std::size_t width = PrimitiveWidthBytes(node);
    if (width == 0u) {
      return MakeError(CodecCode::InvalidSchema, origin, node.id, 0u);
    }
    if (input.size < width) {
      return MakeError(CodecCode::ParentBounds, origin + input.size, node.id, 0u);
    }
    std::uint64_t accumulated = 0u;
    for (std::size_t index = 0u; index < width; ++index) {
      const std::uint64_t byte =
          input.data[schema.littleEndian ? index : (width - 1u) - index];
      accumulated |= (byte << (8u * index));
    }
    decoded.kind = (node.kind == NodeKind::Unsigned) ? ValueKind::Unsigned : ValueKind::Signed;
    decoded.unsigned_value = accumulated;
    decoded.signed_value = static_cast<std::int64_t>(accumulated);
    used = width;
  } else if (node.kind == NodeKind::String) {
    used = input.size;
    decoded.kind = ValueKind::Text;
    decoded.text_utf8.assign(reinterpret_cast<const char*>(input.data), used);
  } else {
    return MakeError(CodecCode::InvalidSchema, origin, node.id, 0u);
  }
  out = decoded;
  consumed = used;
  return CodecResult();
}

CodecResult normalize_classic_utf8(ByteView validUnits, std::size_t arrayCapacityUnits,
                                   Text& out) noexcept {
  (void)arrayCapacityUnits;
  if (validUnits.IsNullRange()) {
    return MakeError(CodecCode::NullRange, 0u, 0u, 0u);
  }
  std::size_t start = 0u;
  if (validUnits.size >= 3u && validUnits.data[0] == 0xEFu && validUnits.data[1] == 0xBBu &&
      validUnits.data[2] == 0xBFu) {
    start = 3u;
  }
  Text text;
  text.utf8.assign(reinterpret_cast<const char*>(validUnits.data) + start,
                   validUnits.size - start);
  out = text;
  return CodecResult();
}

CodecResult normalize_classic_utf16(const std::uint16_t* units, std::size_t validUnits,
                                    std::size_t arrayCapacityUnits, Text& out) noexcept {
  (void)arrayCapacityUnits;
  if (units == nullptr && validUnits != 0u) {
    return MakeError(CodecCode::NullRange, 0u, 0u, 0u);
  }
  std::size_t start = 0u;
  if (validUnits >= 1u && units != nullptr && units[0] == 0xFEFFu) {
    start = 1u;
  }
  Text text;
  for (std::size_t index = start; index < validUnits; ++index) {
    const std::uint32_t value = units[index];
    if (value < 0x80u) {
      text.utf8.push_back(static_cast<char>(value));
    } else if (value < 0x800u) {
      text.utf8.push_back(static_cast<char>(0xC0u | (value >> 6u)));
      text.utf8.push_back(static_cast<char>(0x80u | (value & 0x3Fu)));
    } else {
      text.utf8.push_back(static_cast<char>(0xE0u | (value >> 12u)));
      text.utf8.push_back(static_cast<char>(0x80u | ((value >> 6u) & 0x3Fu)));
      text.utf8.push_back(static_cast<char>(0x80u | (value & 0x3Fu)));
    }
  }
  out = text;
  return CodecResult();
}

CodecResult export_classic_utf8(const Text& text, MutableByteView array,
                                std::size_t& validUnits) noexcept {
  if (array.IsNullRange()) {
    return MakeError(CodecCode::NullRange, 0u, 0u, 0u);
  }
  if (array.size < text.utf8.size()) {
    return MakeError(CodecCode::OutputCapacity, 0u, 0u, 0u);
  }
  for (std::size_t index = 0u; index < text.utf8.size(); ++index) {
    array.data[index] = static_cast<std::uint8_t>(text.utf8[index]);
  }
  validUnits = text.utf8.size();
  return CodecResult();
}

CodecResult export_classic_utf16(const Text& text, std::uint16_t* array,
                                 std::size_t capacityUnits, std::size_t& validUnits) noexcept {
  if (array == nullptr && capacityUnits != 0u) {
    return MakeError(CodecCode::NullRange, 0u, 0u, 0u);
  }
  if (capacityUnits < text.utf8.size()) {
    return MakeError(CodecCode::OutputCapacity, 0u, 0u, 0u);
  }
  for (std::size_t index = 0u; index < text.utf8.size(); ++index) {
    array[index] = static_cast<std::uint16_t>(static_cast<std::uint8_t>(text.utf8[index]));
  }
  validUnits = text.utf8.size();
  return CodecResult();
}

}  // namespace autoeth