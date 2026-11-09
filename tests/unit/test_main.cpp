// AUTOETH unit test entry point. Each suite exercises the contract behaviours
// named in its file; a failure prints the exact expression and location.
#include <cstddef>
#include <cstdint>
#include <vector>

#include "autoeth/build_info.hpp"
#include "autoeth/check.hpp"
#include "autoeth/classic/Crc.h"
#include "autoeth/classic/E2E.h"
#include "autoeth/codec.hpp"
#include "autoeth/discovery.hpp"
#include "autoeth/e2e.hpp"
#include "autoeth/runtime.hpp"
#include "autoeth/tp.hpp"
#include "autoeth/wire.hpp"
#include "test_check.hpp"

namespace {

void TestCheckedArithmetic() {
  std::uint64_t out = 0u;
  AUTOETH_CHECK(autoeth::CheckedAdd(1u, 2u, out) && out == 3u);
  const std::uint64_t max_value = ~static_cast<std::uint64_t>(0u);
  AUTOETH_CHECK(!autoeth::CheckedAdd(max_value, 1u, out));
  AUTOETH_CHECK(autoeth::CheckedSub(5u, 3u, out) && out == 2u);
  AUTOETH_CHECK(!autoeth::CheckedSub(1u, 2u, out));
  AUTOETH_CHECK(autoeth::CheckedMul(3u, 4u, out) && out == 12u);
  AUTOETH_CHECK(!autoeth::CheckedMul(max_value, 2u, out));
}

void TestBuildInfo() {
  const autoeth::BuildInfo info = autoeth::build_info();
  AUTOETH_CHECK(info.major == 0u && info.minor == 1u && info.patch == 0u);
  AUTOETH_CHECK(info.standard_major == 0x04u && info.standard_minor == 0x0Au);
  AUTOETH_CHECK(info.vendor_id == 0x0000u && !info.vendor_id_assigned);
}

void TestCrc() {
  const uint8 zeroes[4] = {0u, 0u, 0u, 0u};
  // First-call values of the selected algorithms (init/xorout applied).
  AUTOETH_CHECK(Crc_CalculateCRC8(zeroes, 4u, 0u, TRUE) == 0x59u);
  Std_VersionInfoType version;
  Crc_GetVersionInfo(&version);
  AUTOETH_CHECK(version.moduleID == 0x00C9u);
  AUTOETH_CHECK(version.sw_major_version == 0x04u);
}

void TestHeaderCodec() {
  const std::vector<std::uint8_t> frame = {0x12, 0x34, 0x00, 0x01, 0x00, 0x00, 0x00,
                                           0x08, 0x46, 0x60, 0x00, 0x01, 0x01, 0x01,
                                           0x00, 0x00};
  autoeth::Header header;
  std::size_t frame_bytes = 0u;
  const autoeth::Status status =
      autoeth::decode_header(autoeth::ByteView(frame.data(), frame.size()), header, frame_bytes);
  AUTOETH_CHECK(status.IsOk());
  AUTOETH_CHECK(header.service_id == 0x1234u);
  AUTOETH_CHECK(header.method_id == 0x0001u);
  AUTOETH_CHECK(header.length == 8u);
  AUTOETH_CHECK(frame_bytes == 16u);

  autoeth::Header short_header;
  const std::uint8_t tiny[4] = {0u, 0u, 0u, 0u};
  const autoeth::Status short_status =
      autoeth::decode_header(autoeth::ByteView(tiny, 4u), short_header, frame_bytes);
  AUTOETH_CHECK(short_status.IsErr(autoeth::NeedMore));
}

void TestValidateMessage() {
  autoeth::RequestRegistry registry;
  autoeth::ElementDescriptor element;
  element.method_id = 1u;
  element.request_response = true;
  element.role = autoeth::RoleMethodRequest;
  autoeth::ValidationDecision decision;

  autoeth::Header request;
  request.service_id = 0x1234u;
  request.protocol_version = 1u;
  request.message_type = 0x00u;
  request.return_code = 0u;
  AUTOETH_CHECK(autoeth::validate_message(request, element, registry, decision).IsOk());

  // R3.4: ERROR with ReturnCode zero is a local MessageType failure.
  autoeth::Header empty_error = request;
  empty_error.message_type = 0x81u;
  empty_error.return_code = 0u;
  AUTOETH_CHECK(
      autoeth::validate_message(empty_error, element, registry, decision)
          .IsErr(autoeth::MessageType));

  autoeth::Header bad_protocol = request;
  bad_protocol.protocol_version = 2u;
  AUTOETH_CHECK(
      autoeth::validate_message(bad_protocol, element, registry, decision)
          .IsErr(autoeth::ProtocolVersion));

  autoeth::Header reserved_service = request;
  reserved_service.service_id = 0xFFFFu;
  AUTOETH_CHECK(
      autoeth::validate_message(reserved_service, element, registry, decision)
          .IsErr(autoeth::UnknownService));
}

void TestTpBudget() {
  // R5.3: take the minimum before the sixteen-byte alignment.
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(1392u, 1472u) == 1392u);
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(17u, 1472u) == 16u);
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(1392u, 1412u) == 1392u);
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(1392u, 1411u) == 1376u);
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(1392u, 35u) == 0u);
  AUTOETH_CHECK(autoeth::NonFinalSegmentPayloadBytes(0u, 19u) == 0u);
}

void TestCodecAlignment() {
  autoeth::SchemaSpec schema;
  schema.name = "align";
  schema.root = 1u;
  schema.role = autoeth::RoleMethodResponse;
  schema.defaultAlignmentBits = 16u;
  autoeth::NodeSpec node;
  node.id = 1u;
  node.kind = autoeth::NodeKind::Unsigned;
  node.widthBits = 16u;
  schema.nodes.push_back(node);

  autoeth::CompiledSchemaHandle handle;
  AUTOETH_CHECK(autoeth::compile_codec_schema(schema, handle).IsOk());
  AUTOETH_CHECK(handle != nullptr);

  autoeth::LayoutContext context;
  context.default_alignment_bits = 8u;  // legal but different from the schema
  autoeth::Value value;
  value.kind = autoeth::ValueKind::Unsigned;
  value.unsigned_value = 0x1234u;
  std::size_t bytes = 0u;
  const autoeth::CodecResult mismatch =
      autoeth::measure_payload(*handle, value, context, bytes);
  AUTOETH_CHECK(mismatch.code == autoeth::CodecCode::InvalidSchema);

  context.default_alignment_bits = 16u;
  AUTOETH_CHECK(autoeth::measure_payload(*handle, value, context, bytes).IsOk());
  AUTOETH_CHECK(bytes == 2u);

  std::vector<std::uint8_t> buffer(2u, 0u);
  std::size_t written = 0u;
  AUTOETH_CHECK(autoeth::encode_payload(*handle, value, context,
                                        autoeth::MutableByteView(buffer.data(), buffer.size()),
                                        written)
                    .IsOk());
  AUTOETH_CHECK(written == 2u);

  autoeth::Value decoded;
  std::size_t consumed = 0u;
  AUTOETH_CHECK(autoeth::decode_payload(*handle, autoeth::ByteView(buffer.data(), buffer.size()),
                                        context, decoded, consumed)
                    .IsOk());
  AUTOETH_CHECK(consumed == 2u);
}

void TestCodecNullAndCapacity() {
  autoeth::SchemaSpec schema;
  schema.name = "bytes";
  schema.root = 1u;
  schema.role = autoeth::RoleEvent;
  schema.defaultAlignmentBits = 8u;
  autoeth::NodeSpec node;
  node.id = 1u;
  node.kind = autoeth::NodeKind::ByteArray;
  node.maxReceiveCount = 4u;
  node.arrayLengthBytes = 1u;
  schema.nodes.push_back(node);
  autoeth::CompiledSchemaHandle handle;
  AUTOETH_CHECK(autoeth::compile_codec_schema(schema, handle).IsOk());

  autoeth::LayoutContext context;
  std::size_t bytes = 0u;
  autoeth::Value value;
  value.kind = autoeth::ValueKind::ByteArray;
  value.bytes.assign(4u, 0x5Au);
  AUTOETH_CHECK(autoeth::measure_payload(*handle, value, context, bytes).IsOk());
  AUTOETH_CHECK(bytes == 4u);

  std::vector<std::uint8_t> small(2u, 0u);
  std::size_t written = 0u;
  AUTOETH_CHECK(autoeth::encode_payload(*handle, value, context,
                                        autoeth::MutableByteView(small.data(), small.size()),
                                        written)
                    .code == autoeth::CodecCode::OutputCapacity);

  const autoeth::CodecResult null_range =
      autoeth::decode_payload(*handle, autoeth::ByteView(nullptr, 4u), context, value, written);
  AUTOETH_CHECK(null_range.code == autoeth::CodecCode::NullRange);
}

void TestE2ECounterDomains() {
  // R6.5: P02 pre-increments (first value 1); P01 post-increments (first 0).
  std::vector<uint8> p02_buffer(16u, 0u);
  E2E_P02ProtectStateType p02_state;
  E2E_P02ConfigType p02_config;
  AUTOETH_CHECK(E2E_P02ProtectInit(&p02_state) == E2E_E_OK);
  AUTOETH_CHECK(E2E_P02Protect(&p02_config, &p02_state, p02_buffer.data()) == E2E_E_OK);
  AUTOETH_CHECK(p02_buffer[8] == 1u);
  AUTOETH_CHECK(p02_state.Counter == 1u);

  std::vector<uint8> p01_buffer(16u, 0u);
  E2E_P01ProtectStateType p01_state;
  E2E_P01ConfigType p01_config;
  AUTOETH_CHECK(E2E_P01ProtectInit(&p01_state) == E2E_E_OK);
  AUTOETH_CHECK(E2E_P01Protect(&p01_config, &p01_state, p01_buffer.data()) == E2E_E_OK);
  AUTOETH_CHECK(p01_buffer[8] == 0u);
  AUTOETH_CHECK(p01_state.Counter == 1u);

  // P11 uses modulo 15 and never emits the 0x0F counter value.
  E2E_P11ProtectStateType p11_state;
  p11_state.Counter = 14u;
  std::vector<uint8> p11_buffer(16u, 0u);
  E2E_P11ConfigType p11_config;
  AUTOETH_CHECK(E2E_P11Protect(&p11_config, &p11_state, p11_buffer.data(), 16u) == E2E_E_OK);
  AUTOETH_CHECK(p11_buffer[8] == 14u);
  AUTOETH_CHECK(p11_state.Counter == 0u);

  // P22 keeps its own modulo 16 domain.
  E2E_P22ProtectStateType p22_state;
  p22_state.Counter = 15u;
  std::vector<uint8> p22_buffer(16u, 0u);
  E2E_P22ConfigType p22_config;
  AUTOETH_CHECK(E2E_P22Protect(&p22_config, &p22_state, p22_buffer.data(), 16u) == E2E_E_OK);
  AUTOETH_CHECK(p22_state.Counter == 0u);
}

void TestE2ENullInputs() {
  E2E_P04ProtectStateType state;
  AUTOETH_CHECK(E2E_P04ProtectInit(NULL_PTR) == E2E_E_INPUTERR_NULL);
  AUTOETH_CHECK(E2E_P04ProtectInit(&state) == E2E_E_OK);
  E2E_P04ConfigType config;
  AUTOETH_CHECK(E2E_P04Protect(&config, &state, NULL_PTR, 16u) == E2E_E_INPUTERR_NULL);
}

void TestE2EWideNarrowing() {
  autoeth::e2e::P08ConfigWide wide;
  wide.data_id = 7u;
  // 64-bit literals: 8u * 536870912u would wrap in unsigned int (R0.2.1).
  wide.max_data_length_bits = 8ull * 536870912ull;
  wide.min_data_length_bits = 8ull * 96ull;
  wide.offset_bits = 8ull * 8ull;
  wide.max_delta_counter = 1u;
  E2E_P08ConfigType classic;
  classic.DataID = 0xAAAu;
  AUTOETH_CHECK(autoeth::e2e::to_classic_p08_config(wide, classic).IsErr(
      autoeth::UnrepresentableClassicBitLength));

  // Output must be untouched after the rejected narrowing (R6.6).
  AUTOETH_CHECK(classic.DataID == 0xAAAu);

  wide.max_data_length_bits = 8ull * 32768ull;
  AUTOETH_CHECK(autoeth::e2e::to_classic_p08_config(wide, classic).IsOk());
  AUTOETH_CHECK(classic.MaxDataLength == 32768u);
}

void TestSmConfigMapping() {
  E2E_SMConfigType classic;
  classic.WindowSizeValid = 10u;
  classic.MinOkStateInit = 2u;
  classic.MaxErrorStateInit = 2u;
  classic.MinOkStateValid = 5u;
  classic.MaxErrorStateValid = 5u;
  classic.MinOkStateInvalid = 5u;
  classic.MaxErrorStateInvalid = 5u;
  classic.WindowSizeInit = 3u;
  classic.WindowSizeInvalid = 3u;
  classic.ClearToInvalid = TRUE;
  classic.transitToInvalidExtended = FALSE;
  const autoeth::e2e::SmConfig native = autoeth::e2e::from_classic_sm_config(classic);
  AUTOETH_CHECK(native.window_size_valid == 10u);
  AUTOETH_CHECK(native.clear_from_valid_to_invalid);
  AUTOETH_CHECK(!native.combined_no_data_init_count);
}

void TestTpSegmentAndReassembly() {
  autoeth::Message message;
  message.payload.assign(40u, 0x33u);
  autoeth::TpTxConfig config;
  config.max_segment_payload_bytes = 16u;
  config.datagram_capacity_bytes = 1472u;
  autoeth::SegmentSink sink;
  AUTOETH_CHECK(autoeth::segment(message, config, sink).IsOk());
  AUTOETH_CHECK(sink.ReservedBytes() >= message.payload.size());

  autoeth::TpTxConfig rejected;
  rejected.max_segment_payload_bytes = 4u;
  rejected.datagram_capacity_bytes = 20u;
  autoeth::SegmentSink rejected_sink;
  AUTOETH_CHECK(autoeth::segment(message, rejected, rejected_sink)
                    .IsErr(autoeth::InvalidConfig));

  autoeth::TpReceiver receiver;
  AUTOETH_CHECK(receiver.AssemblyCount() == 0u);
  std::vector<std::uint8_t> frame(20u + 8u, 0u);
  frame[7] = 8u;      // total length 8
  frame[15] = 0x00u;  // last segment
  autoeth::Peer peer;
  autoeth::ReceiveOutcome outcome;
  AUTOETH_CHECK(receiver.accept(peer, autoeth::ByteView(frame.data(), frame.size()), 1000u,
                                outcome)
                    .IsOk());
  AUTOETH_CHECK(outcome.has_message);

  autoeth::TpReceiver expiry_receiver;
  std::vector<std::uint8_t> partial(20u + 8u, 0u);
  partial[7] = 32u;
  partial[15] = 0x01u;  // more segments follow
  AUTOETH_CHECK(expiry_receiver.accept(peer, autoeth::ByteView(partial.data(), partial.size()),
                                       1000u, outcome)
                    .IsOk());
  AUTOETH_CHECK(expiry_receiver.AssemblyCount() == 1u);
  autoeth::ExpirySink expiry;
  expiry_receiver.expire(5000000000u, expiry);
  AUTOETH_CHECK(expiry.cancel_count == 1u);
  AUTOETH_CHECK(expiry_receiver.AssemblyCount() == 0u);
}

void TestDiscovery() {
  autoeth::SdOfferSnapshot first;
  first.service_id = 0x1234u;
  first.instance_id = 1u;
  first.major_version = 1u;
  autoeth::SdOfferSnapshot second;
  second.service_id = 0x1234u;
  second.instance_id = 2u;
  second.major_version = 1u;
  std::vector<autoeth::SdOfferSnapshot> offers;
  offers.push_back(first);
  offers.push_back(second);

  autoeth::ServiceQuery query;
  query.service_id = 0x1234u;
  query.major_version = 1u;
  query.exact_instance = true;
  query.instance_id = 2u;
  autoeth::RandomDraw draw;
  autoeth::SdOfferSnapshot selected;
  AUTOETH_CHECK(autoeth::select_service(query, offers, draw, selected).IsOk());
  AUTOETH_CHECK(selected.instance_id == 2u);

  query.instance_id = 9u;
  AUTOETH_CHECK(autoeth::select_service(query, offers, draw, selected)
                    .IsErr(autoeth::NoService));

  autoeth::SdEndpoint endpoint;
  AUTOETH_CHECK(!autoeth::IsActionableEndpoint(endpoint));
  endpoint.port = 30501u;
  AUTOETH_CHECK(autoeth::IsActionableEndpoint(endpoint));
}

void TestRuntimeAdmission() {
  autoeth::RuntimeConfig config;
  config.schema_version = 1u;
  autoeth::RuntimeHandle runtime;
  AUTOETH_CHECK(autoeth::create_runtime(config, runtime).IsOk());
  AUTOETH_CHECK(runtime != nullptr);

  autoeth::RuntimeConfig bad;
  bad.schema_version = 2u;
  autoeth::RuntimeHandle rejected;
  AUTOETH_CHECK(autoeth::create_runtime(bad, rejected).IsErr(autoeth::InvalidConfig));
  AUTOETH_CHECK(rejected == nullptr);

  autoeth::MethodRef method;
  method.method_id = 1u;
  autoeth::Value arguments;
  autoeth::CallHandle handle;
  autoeth::Completion completion = [](const autoeth::CompletionSnapshot&) {};
  AUTOETH_CHECK(runtime->call(method, arguments, autoeth::DurationNs(1000000u), completion,
                              handle)
                    .IsOk());
  AUTOETH_CHECK(handle.slot != 0u && handle.generation != 0u);

  autoeth::SubscriptionHandle subscription;
  autoeth::EventGroupRef group;
  group.group_id = 1u;
  autoeth::SubscriptionOptions options;
  autoeth::EventCallback callback = [](const autoeth::EventSnapshot&) {};
  AUTOETH_CHECK(runtime->subscribe(group, options, callback, subscription).IsOk());

  autoeth::RuntimeSnapshot snapshot;
  AUTOETH_CHECK(runtime->snapshot(snapshot).IsOk());
  AUTOETH_CHECK(runtime->stop(autoeth::DurationNs(0u)).IsOk());
}

void TestConfigLoader() {
  autoeth::RuntimeConfig config;
  autoeth::ConfigDiagnostic diagnostic;
  const std::string empty_object = "{}";
  const autoeth::ByteView view(reinterpret_cast<const std::uint8_t*>(empty_object.data()),
                               empty_object.size());
  // schema_version remains 0, so the version stage rejects it.
  AUTOETH_CHECK(autoeth::load_runtime_config(view, ".", config, &diagnostic)
                    .IsErr(autoeth::InvalidConfig));
  AUTOETH_CHECK(diagnostic.code == autoeth::VersionMismatch);

  const std::string not_object = "[]";
  const autoeth::ByteView array_view(reinterpret_cast<const std::uint8_t*>(not_object.data()),
                                     not_object.size());
  AUTOETH_CHECK(autoeth::load_runtime_config(array_view, ".", config, &diagnostic)
                    .IsErr(autoeth::InvalidConfig));
}

}  // namespace

void RunModelCheckingDerivedTests();

int main() {
  TestCheckedArithmetic();
  TestBuildInfo();
  TestCrc();
  TestHeaderCodec();
  TestValidateMessage();
  TestTpBudget();
  TestCodecAlignment();
  TestCodecNullAndCapacity();
  TestE2ECounterDomains();
  TestE2ENullInputs();
  TestE2EWideNarrowing();
  TestSmConfigMapping();
  TestTpSegmentAndReassembly();
  TestDiscovery();
  TestRuntimeAdmission();
  TestConfigLoader();
  RunModelCheckingDerivedTests();
  return autoeth_test::Summary("autoeth_unit");
}