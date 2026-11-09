// AUTOETH Native Runtime surface. R8.1: exact signatures, fixed callbacks and
// owning configuration aggregates. Private storage is chosen here.
#ifndef AUTOETH_RUNTIME_HPP
#define AUTOETH_RUNTIME_HPP

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "autoeth/codec.hpp"
#include "autoeth/discovery.hpp"
#include "autoeth/e2e.hpp"
#include "autoeth/security.hpp"
#include "autoeth/tp.hpp"
#include "autoeth/types.hpp"
#include "autoeth/wire.hpp"

namespace autoeth {

struct RuntimeConfig;
class Runtime;

struct ResourceConfig {
  std::uint64_t max_frame_bytes;
  std::uint64_t max_owned_payload_bytes;
  std::uint32_t max_commands;
  std::uint32_t max_active_rpcs;
  std::uint32_t max_tcp_connections;
  std::uint32_t max_tp_rx_contexts;
  std::uint32_t max_visible_sd_instances;
  std::uint32_t max_server_subscriptions;
  std::uint32_t max_schema_nodes;
  std::uint32_t max_schema_depth;
  std::uint32_t max_e2e_rx_contexts;
  std::uint32_t max_udp_sockets;
  std::uint32_t max_multicast_memberships;
  std::uint32_t max_trace_records;
  std::uint64_t trace_payload_bytes;

  ResourceConfig() noexcept
      : max_frame_bytes(8388608u), max_owned_payload_bytes(268435456u), max_commands(4096u),
        max_active_rpcs(4096u), max_tcp_connections(128u), max_tp_rx_contexts(128u),
        max_visible_sd_instances(4096u), max_server_subscriptions(4096u), max_schema_nodes(4096u),
        max_schema_depth(64u), max_e2e_rx_contexts(4096u), max_udp_sockets(128u),
        max_multicast_memberships(256u), max_trace_records(4096u), trace_payload_bytes(1048576u) {}
};

struct SchedulingConfig {
  std::uint64_t main_period_ns;
  std::uint64_t shutdown_drain_ns;
  std::uint64_t transport_abort_cleanup_ns;
  std::uint64_t connect_retry_ns;
  std::uint64_t rpc_timeout_ns;

  SchedulingConfig() noexcept
      : main_period_ns(1000000u), shutdown_drain_ns(2000000000u),
        transport_abort_cleanup_ns(1000000000u), connect_retry_ns(100000000u),
        rpc_timeout_ns(1000000000u) {}
};

struct CorrelationConfig {
  std::uint64_t retirement_horizon_ns;
  bool fresh_context_asserted;

  CorrelationConfig() noexcept : retirement_horizon_ns(60000000000u), fresh_context_asserted(false) {}
};

struct CaptureConfig {
  bool enabled;
  std::string output_path;

  CaptureConfig() noexcept : enabled(false) {}
};

struct InterfaceConfig {
  std::string id;
  std::string name;
  std::string local_address;
  std::string address_family;
  std::uint32_t interface_index;
  std::uint32_t scope_id;
  std::uint16_t sd_port;
  std::string sd_multicast_address;
  bool loopback_enabled;
  std::uint8_t multicast_hops;

  InterfaceConfig() noexcept
      : interface_index(0u), scope_id(0u), sd_port(30490u), loopback_enabled(false),
        multicast_hops(1u) {}
};

struct EndpointConfig {
  std::string id;
  std::string interface_id;
  std::string transport;
  std::uint16_t local_port;
  std::string tcp_role;
  bool tcp_no_delay;
  std::string remote_address;
  std::uint16_t remote_port;
  bool has_remote;
  std::string tp_strategy;
  std::uint32_t max_segment_payload_bytes;
  std::uint64_t rx_timeout_ns;
  bool changed_overlap_reject;
  std::uint32_t max_udp_datagram_bytes;
  std::uint64_t udp_bundle_delay_ns;
  std::uint16_t max_bundle_messages;
  bool multicast_endpoint;
  bool requires_security;
  std::string security_provider;

  EndpointConfig() noexcept
      : local_port(0u), tcp_no_delay(true), remote_port(0u), has_remote(false),
        max_segment_payload_bytes(1392u), rx_timeout_ns(1000000000u),
        changed_overlap_reject(false), max_udp_datagram_bytes(1472u),
        udp_bundle_delay_ns(1000000u), max_bundle_messages(64u), multicast_endpoint(false),
        requires_security(false) {}
};

struct DetailedErrorSpec {
  std::uint8_t application_error = 0;
  std::string schema;
};

struct MethodConfig {
  std::uint16_t method_id;
  std::string name;
  std::string kind;
  std::string request_schema;
  std::string response_schema;
  bool has_response_schema;
  std::vector<std::string> transports;
  bool udp_segment;
  std::vector<std::uint8_t> application_errors;
  std::vector<DetailedErrorSpec> detailed_errors;
  std::string e2e_channel;
  bool unprotected;

  MethodConfig() noexcept
      : method_id(0u), has_response_schema(false), udp_segment(false), unprotected(false) {}
};

struct EventConfig {
  std::uint16_t event_id;
  std::string name;
  std::string schema;
  std::vector<std::uint16_t> group_ids;
  std::vector<std::string> transports;
  std::string e2e_channel;
  bool pure_event;

  EventConfig() noexcept : event_id(0u), pure_event(false) {}
};

struct FieldConfig {
  std::string name;
  std::string schema;
  std::uint16_t getter;
  std::uint16_t setter;
  std::uint16_t notifier;
  bool readable;
  bool writable;
  bool notifiable;
  bool has_initial_value;
  Value initial_value;

  FieldConfig() noexcept
      : getter(0u), setter(0u), notifier(0u), readable(false), writable(false), notifiable(false),
        has_initial_value(false) {}
};

struct EventGroupConfig {
  std::uint16_t group_id;
  std::vector<std::uint16_t> event_ids;
  std::uint32_t ttl_seconds;
  std::uint32_t multicast_threshold;
  std::string server_multicast_endpoint;

  EventGroupConfig() noexcept : group_id(0u), ttl_seconds(3u), multicast_threshold(0u) {}
};

struct ServiceDescriptor {
  std::string name;
  std::uint16_t service_id;
  std::uint16_t instance_id;
  std::uint8_t major;
  std::uint32_t minor;
  std::vector<MethodConfig> methods;
  std::vector<EventConfig> events;
  std::vector<FieldConfig> fields;
  std::vector<EventGroupConfig> event_groups;

  ServiceDescriptor() noexcept : service_id(0u), instance_id(0u), major(0u), minor(0u) {}
};

struct ServerTimingConfig {
  std::uint64_t initial_delay_min_ns;
  std::uint64_t initial_delay_max_ns;
  std::uint64_t repetition_base_ns;
  std::uint8_t repetitions;
  std::uint64_t cyclic_offer_ns;
  std::uint64_t request_response_min_ns;
  std::uint64_t request_response_max_ns;
  std::uint32_t ttl_seconds;
  std::string find_response_mode;
  std::string discovery_mode;

  ServerTimingConfig() noexcept
      : initial_delay_min_ns(0u), initial_delay_max_ns(0u), repetition_base_ns(0u), repetitions(0u),
        cyclic_offer_ns(0u), request_response_min_ns(0u), request_response_max_ns(0u),
        ttl_seconds(3u), find_response_mode("BASIC_UNICAST"), discovery_mode("DYNAMIC") {}
};

struct ClientTimingConfig {
  std::uint64_t initial_find_min_ns;
  std::uint64_t initial_find_max_ns;
  std::uint64_t repetition_base_ns;
  std::uint8_t repetitions;
  std::uint64_t request_response_min_ns;
  std::uint64_t request_response_max_ns;
  std::uint64_t subscription_retry_delay_ns;
  std::uint32_t subscription_retry_max;
  bool retry_infinite;
  std::uint32_t subscription_ttl_seconds;
  std::string mode;
  std::string discovery_mode;

  ClientTimingConfig() noexcept
      : initial_find_min_ns(0u), initial_find_max_ns(0u), repetition_base_ns(0u), repetitions(0u),
        request_response_min_ns(0u), request_response_max_ns(0u),
        subscription_retry_delay_ns(10000000u), subscription_retry_max(0u),
        retry_infinite(false), subscription_ttl_seconds(3u), mode("ACTIVE"),
        discovery_mode("DYNAMIC") {}
};

struct ProvidedServiceConfig {
  ServiceDescriptor descriptor;
  std::vector<std::string> endpoints;
  bool auto_available;
  ServerTimingConfig server_timing;
  std::string notification_session_handling;

  ProvidedServiceConfig() noexcept : auto_available(true), notification_session_handling("INACTIVE") {}
};

struct RequestedServiceConfig {
  ServiceDescriptor descriptor;
  std::string query_handle;
  std::vector<std::string> endpoints;
  ServiceQuery query;
  bool auto_require;
  std::vector<CapabilityCriterion> capabilities;
  ClientTimingConfig client_timing;

  RequestedServiceConfig() noexcept : auto_require(true) {}
};

struct E2eChannelConfig {
  std::string channel_id;
  std::string profile;
  std::string configuration_surface;
  std::map<std::string, std::string> config;
  bool legacy_compatibility;
  std::string behavior;
  std::uint32_t source_id;
  bool has_state_machine;
  e2e::SmConfig state_machine;
  bool disable_check;
  bool has_disable_check;

  E2eChannelConfig() noexcept
      : legacy_compatibility(false), source_id(0u), has_state_machine(false),
        disable_check(false), has_disable_check(false) {}
};

struct SecurityConfig {
  std::vector<std::string> providers;
};

struct CompatibilityConfig {
  std::string sd_profile;
  bool classic_host;

  CompatibilityConfig() noexcept : sd_profile("FO_R24_11"), classic_host(false) {}
};

struct RuntimeConfig {
  std::uint32_t schema_version;
  std::string node_name;
  std::uint16_t client_id;
  ResourceConfig resources;
  SchedulingConfig scheduling;
  CorrelationConfig correlation;
  std::vector<InterfaceConfig> interfaces;
  std::vector<EndpointConfig> endpoints;
  std::vector<SchemaSpec> schemas;
  std::vector<ProvidedServiceConfig> provided_services;
  std::vector<RequestedServiceConfig> requested_services;
  std::vector<E2eChannelConfig> e2e_channels;
  SecurityConfig security;
  CompatibilityConfig compatibility;
  CaptureConfig capture;

  RuntimeConfig() noexcept : schema_version(0u), client_id(0u) {}
};

enum ConfigDetail : std::uint16_t {
  DuplicateKey = 1,
  UnknownKey = 2,
  VersionMismatch = 3,
  TypeMismatch = 4,
  RangeMismatch = 5,
  ReferenceMismatch = 6,
  CycleDetected = 7,
  RoleMismatch = 8,
  UnsupportedValue = 9,
  DepthExceeded = 10,
  CountMismatch = 11,
  ProfileGeometry = 12,
  ContextMismatch = 13,
  IoFailure = 14
};

struct ConfigDiagnostic {
  ConfigDetail code;
  std::string file;
  std::string json_path;
  std::uint64_t byte_offset;
  bool path_complete;

  ConfigDiagnostic() noexcept
      : code(DuplicateKey), byte_offset(0u), path_complete(false) {}
};

struct CompiledSchemaHolder;

struct OfferSnapshot {
  SdOfferSnapshot offer;
};

struct EventSnapshot {
  Header header;
  Value value;
  std::uint8_t e2e_source_call_return;
  std::uint8_t e2e_raw_status;
  std::uint8_t e2e_generic_status;
  std::uint8_t e2e_sm_state;
  bool e2e_sm_enabled;
  bool e2e_metadata_valid;
  std::uint32_t e2e_received_counter;
  std::uint32_t e2e_received_source_id;

  EventSnapshot() noexcept
      : e2e_source_call_return(0u), e2e_raw_status(0u), e2e_generic_status(0u), e2e_sm_state(0u),
        e2e_sm_enabled(false), e2e_metadata_valid(false), e2e_received_counter(0u),
        e2e_received_source_id(0u) {}
};

struct SendSnapshot {
  Status status;
  std::uint64_t accepted_count;
  std::uint64_t owned_count;
  std::uint64_t kernel_progress_count;
  std::uint64_t connection_generation;
  MonotonicNs completed_at;

  SendSnapshot() noexcept
      : accepted_count(0u), owned_count(0u), kernel_progress_count(0u), connection_generation(0u),
        completed_at(MonotonicNs(0u)) {}
};

struct AvailabilitySnapshot {
  bool requested;
  bool current;
  std::vector<OfferSnapshot> offers;

  AvailabilitySnapshot() noexcept : requested(false), current(false) {}
};

struct E2eObservation {
  std::uint8_t source_call_return;
  std::uint8_t raw_status;
  std::uint8_t generic_status;
  std::uint8_t sm_state;
  bool sm_enabled;
  bool metadata_valid;
  std::uint32_t received_counter;
  std::uint32_t received_source_id;

  E2eObservation() noexcept
      : source_call_return(0u), raw_status(0u), generic_status(0u), sm_state(0u),
        sm_enabled(false), metadata_valid(false), received_counter(0u), received_source_id(0u) {}
};

enum ResultKind : std::uint8_t {
  Success = 0,
  RemoteApplicationError = 1,
  RemoteProtocolError = 2,
  TimeoutKind = 3,
  CancelledKind = 4,
  LocalFailure = 5
};

struct CompletionSnapshot {
  Status status;
  ResultKind kind;
  bool output_present;
  bool checked;
  Value output;
  std::uint8_t wire_return_code;
  std::uint8_t application_error;
  E2eObservation e2e;
  MonotonicNs completed_at;
  CallHandle call;

  CompletionSnapshot() noexcept
      : kind(LocalFailure), output_present(false), checked(false), wire_return_code(0u),
        application_error(0u), completed_at(MonotonicNs(0u)) {}
};

struct RequestContext {
  std::uint32_t peer_address;
  std::uint32_t interface_index;
  std::uint16_t service_id;
  std::uint16_t method_id;
  std::uint16_t client_id;
  std::uint16_t session_id;
  std::uint64_t correlation;
  E2eObservation e2e;

  RequestContext() noexcept
      : peer_address(0u), interface_index(0u), service_id(0u), method_id(0u), client_id(0u),
        session_id(0u), correlation(0u) {}
};

struct MethodReply {
  std::uint8_t application_error;
  Value output;

  MethodReply() noexcept : application_error(0u) {}
};

using Completion = std::function<void(const CompletionSnapshot&)>;
using SendCompletion = std::function<void(const SendSnapshot&)>;
using AvailabilityCallback = std::function<void(const AvailabilitySnapshot&)>;
using EventCallback = std::function<void(const EventSnapshot&)>;
using MethodHandler = std::function<MethodReply(const RequestContext&, const Value&)>;
using NoReturnHandler = std::function<void(const RequestContext&, const Value&)>;

enum Transport : std::uint8_t { TransportUdp = 0, TransportTcp = 1 };

struct SubscriptionOptions {
  DurationNs retry_delay;
  std::uint32_t retry_max;
  bool retry_infinite;
  std::uint32_t ttl_seconds;
  Transport transport;
  std::string endpoint;

  SubscriptionOptions() noexcept
      : retry_delay(DurationNs(10000000u)), retry_max(0u), retry_infinite(false),
        ttl_seconds(3u), transport(TransportUdp) {}
};

struct MethodRef {
  SdOfferSnapshot offer;
  std::uint16_t method_id;
  Transport transport;

  MethodRef() noexcept : method_id(0u), transport(TransportUdp) {}
};

struct EventRef {
  SdOfferSnapshot offer;
  std::uint16_t event_id;
  Transport transport;

  EventRef() noexcept : event_id(0u), transport(TransportUdp) {}
};

struct FieldRef {
  SdOfferSnapshot offer;
  std::uint16_t field_id;
  Transport transport;

  FieldRef() noexcept : field_id(0u), transport(TransportUdp) {}
};

struct EventGroupRef {
  SdOfferSnapshot offer;
  std::uint16_t group_id;
  Transport transport;

  EventGroupRef() noexcept : group_id(0u), transport(TransportUdp) {}
};

struct ServiceRegistration {
  ServiceDescriptor descriptor;
  std::vector<std::string> endpoints;
  std::map<std::uint16_t, MethodHandler> method_handlers;
  std::map<std::uint16_t, NoReturnHandler> no_return_handlers;

  ServiceRegistration() = default;
};

struct DiagnosticRecord {
  std::uint64_t sequence;
  std::uint64_t monotonic_ns;
  bool has_operation;
  std::uint32_t operation_slot;
  std::uint64_t operation_generation;
  bool has_codec_result;
  CodecResult codec_result;
  Status status;

  DiagnosticRecord() noexcept
      : sequence(0u), monotonic_ns(0u), has_operation(false), operation_slot(0u),
        operation_generation(0u), has_codec_result(false) {}
};

struct RuntimeSnapshot {
  std::uint64_t sequence;
  MonotonicNs published_at;
  std::vector<OfferSnapshot> requested_services;
  std::vector<OfferSnapshot> current_services;
  std::vector<CallHandle> calls;
  std::uint64_t connection_generation;
  std::uint64_t payload_budget_used;
  std::uint64_t payload_budget_high_water;
  std::vector<DiagnosticRecord> diagnostics;
  std::uint64_t lost_records;

  RuntimeSnapshot() noexcept
      : sequence(0u), published_at(MonotonicNs(0u)), connection_generation(0u),
        payload_budget_used(0u), payload_budget_high_water(0u), lost_records(0u) {}
};

using RuntimeHandle = std::unique_ptr<Runtime>;

Status load_runtime_config(ByteView utf8, const std::string& base_directory, RuntimeConfig& out,
                           ConfigDiagnostic* diagnostic = nullptr) noexcept;

class Runtime {
 public:
  Runtime() noexcept;
  virtual ~Runtime() noexcept;
  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;

  virtual Status start() noexcept;

  virtual Status offer_service(const ServiceRegistration& registration, ServiceHandle& out) noexcept;
  virtual Status set_available(ServiceHandle service, bool available) noexcept;
  virtual Status request_service(const ServiceQuery& query, AvailabilityCallback callback,
                                 RequestHandle& out) noexcept;
  virtual Status release_service(RequestHandle request) noexcept;
  virtual Status call(const MethodRef& method, const Value& arguments, DurationNs timeout,
                      Completion completion, CallHandle& out) noexcept;
  virtual Status send_no_return(const MethodRef& method, const Value& arguments,
                                SendCompletion completion) noexcept;
  virtual Status cancel(CallHandle callHandle) noexcept;
  virtual Status completion_snapshot(CallHandle callHandle, CompletionSnapshot& out) noexcept;
  virtual Status release_call_handle(CallHandle callHandle) noexcept;
  virtual Status subscribe(const EventGroupRef& group, const SubscriptionOptions& options,
                           EventCallback callback, SubscriptionHandle& out) noexcept;
  virtual Status release_subscription(SubscriptionHandle subscription) noexcept;
  virtual Status get_field(const FieldRef& field, DurationNs timeout, Completion completion,
                           CallHandle& out) noexcept;
  virtual Status set_field(const FieldRef& field, const Value& value, DurationNs timeout,
                           Completion completion, CallHandle& out) noexcept;
  virtual Status publish_event(ServiceHandle service, const EventRef& event, const Value& value,
                               SendCompletion completion) noexcept;
  virtual Status update_field(ServiceHandle service, const FieldRef& field, const Value& value,
                              SendCompletion completion) noexcept;
  virtual Status snapshot(RuntimeSnapshot& out) noexcept;
  virtual Status request_stop(DurationNs drain) noexcept;
  virtual Status stop(DurationNs drain) noexcept;
};

Status create_runtime(const RuntimeConfig& config, RuntimeHandle& out) noexcept;

}  // namespace autoeth

#endif