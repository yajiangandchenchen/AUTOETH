// AUTOETH Native codec surface: SchemaSpec/NodeSpec, Value, CodecResult.
// R2.27 (14 CodecCode values) and R2.1 (Value fields) are contractual.
#ifndef AUTOETH_CODEC_HPP
#define AUTOETH_CODEC_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "autoeth/types.hpp"

namespace autoeth {

// Scoped enumeration: R2.27 numeric values are contractual, the scope name is
// a local C++ convenience so they cannot collide with the local Errc values.
enum class CodecCode : std::uint16_t {
  Ok = 0,
  NullRange = 1,
  InvalidSchema = 2,
  ValueType = 3,
  ValueRange = 4,
  InvalidText = 5,
  LengthOverflow = 6,
  ParentBounds = 7,
  DuplicateKnownDataId = 8,
  KnownWireType = 9,
  MissingRequired = 10,
  UnknownUnionSelector = 11,
  OutputCapacity = 12,
  Resource = 13
};

struct CodecResult {
  CodecCode code;
  std::uint64_t byteOffset;
  std::uint32_t nodeId;
  std::uint16_t dataId;

  CodecResult() noexcept
      : code(CodecCode::Ok), byteOffset(0u), nodeId(0u), dataId(0u) {}

  static CodecResult Success() noexcept { return CodecResult(); }
  bool IsOk() const noexcept { return code == CodecCode::Ok; }
};

enum class ValueKind : std::uint8_t {
  Bool,
  Unsigned,
  Signed,
  Float32Bits,
  Float64Bits,
  Text,
  ByteArray,
  Struct,
  Array,
  Union
};

struct Value {
  ValueKind kind;
  bool boolean_value;
  std::uint64_t unsigned_value;
  std::int64_t signed_value;
  std::uint32_t float32_bits;
  std::uint64_t float64_bits;
  std::string text_utf8;
  std::vector<std::uint8_t> bytes;
  std::vector<Value> items;
  std::vector<std::uint32_t> member_ids;
  std::vector<std::uint8_t> member_present;
  std::uint32_t selector;

  Value() noexcept
      : kind(ValueKind::Unsigned), boolean_value(false), unsigned_value(0u), signed_value(0),
        float32_bits(0u), float64_bits(0u), selector(0u) {}
};

enum SchemaRole : std::uint8_t {
  RoleEvent = 0,
  RoleField = 1,
  RoleMethodRequest = 2,
  RoleMethodResponse = 3
};

enum ValuePresence : std::uint8_t { NoEquals = 0, EmptyValue = 1, HasValue = 2 };

struct LayoutContext {
  std::uint64_t absolute_payload_origin_bytes;
  std::uint16_t default_alignment_bits;

  LayoutContext() noexcept : absolute_payload_origin_bytes(0u), default_alignment_bits(8u) {}
};

struct MemberSpec {
  std::string name;
  std::uint32_t node;
  bool optional;
  std::uint16_t dataId;
  bool has_reception_default;
  Value reception_default;

  MemberSpec() noexcept : node(0u), optional(false), dataId(0u), has_reception_default(false) {}
};

struct VariantSpec {
  std::uint32_t selector;
  std::uint32_t node;

  VariantSpec() noexcept : selector(0u), node(0u) {}
};

enum class NodeKind : std::uint8_t {
  Bool,
  Unsigned,
  Signed,
  Float32Bits,
  Float64Bits,
  Struct,
  Array,
  ByteArray,
  String,
  Union
};

struct NodeSpec {
  std::uint32_t id;
  NodeKind kind;
  std::uint32_t widthBits;
  std::vector<MemberSpec> members;
  std::uint8_t structLengthBytes;
  bool tagged;
  std::uint32_t element;
  std::uint32_t fixedCount;
  std::uint32_t maxReceiveCount;
  std::uint8_t arrayLengthBytes;
  bool has_missingElementDefaults;
  std::vector<Value> missingElementDefaults;
  bool utf16;
  std::uint32_t maxWireBytes;
  std::uint32_t fixedWireBytes;
  std::uint32_t maxUnicodeScalars;
  std::uint8_t selectorWidthBytes;
  std::vector<VariantSpec> variants;
  std::uint8_t unionLengthBytes;
  std::uint32_t nonemptyPayloadExtentBytes;

  NodeSpec() noexcept
      : id(0u), kind(NodeKind::Unsigned), widthBits(0u), structLengthBytes(0u), tagged(false),
        element(0u), fixedCount(0u), maxReceiveCount(0u), arrayLengthBytes(0u),
        has_missingElementDefaults(false), utf16(false), maxWireBytes(0u), fixedWireBytes(0u),
        maxUnicodeScalars(0u), selectorWidthBytes(0u), unionLengthBytes(0u),
        nonemptyPayloadExtentBytes(0u) {}
};

struct SchemaSpec {
  std::string name;
  std::uint32_t root;
  SchemaRole role;
  bool argumentSet;
  bool littleEndian;
  std::uint8_t tlvStaticLengthBytes;
  bool dynamicTaggedLength;
  std::uint16_t defaultAlignmentBits;
  std::vector<NodeSpec> nodes;

  SchemaSpec() noexcept
      : root(0u), role(RoleEvent), argumentSet(false), littleEndian(false),
        tlvStaticLengthBytes(0u), dynamicTaggedLength(false), defaultAlignmentBits(8u) {}
};

struct CompiledNode;

struct CompiledSchema {
  std::string name;
  SchemaRole role;
  bool argumentSet;
  bool littleEndian;
  std::uint8_t tlvStaticLengthBytes;
  std::uint16_t defaultAlignmentBits;
  std::shared_ptr<const CompiledNode> root;
};

using CompiledSchemaHandle = std::shared_ptr<const CompiledSchema>;

struct Text {
  std::string utf8;

  Text() = default;
};

CodecResult compile_codec_schema(const SchemaSpec& schema, CompiledSchemaHandle& out) noexcept;
CodecResult measure_payload(const CompiledSchema& schema, const Value& value,
                            const LayoutContext& context, std::size_t& bytes) noexcept;
CodecResult encode_payload(const CompiledSchema& schema, const Value& value,
                           const LayoutContext& context, MutableByteView output,
                           std::size_t& written) noexcept;
CodecResult decode_payload(const CompiledSchema& schema, ByteView input,
                           const LayoutContext& context, Value& out,
                           std::size_t& consumed) noexcept;
CodecResult normalize_classic_utf8(ByteView validUnits, std::size_t arrayCapacityUnits,
                                   Text& out) noexcept;
CodecResult normalize_classic_utf16(const std::uint16_t* units, std::size_t validUnits,
                                    std::size_t arrayCapacityUnits, Text& out) noexcept;
CodecResult export_classic_utf8(const Text& text, MutableByteView array,
                                std::size_t& validUnits) noexcept;
CodecResult export_classic_utf16(const Text& text, std::uint16_t* array,
                                 std::size_t capacityUnits, std::size_t& validUnits) noexcept;

}  // namespace autoeth

#endif