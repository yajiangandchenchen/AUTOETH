# SUPPLIED CUSTOMER DESIGN/API: Native types and callable contracts

Provided equally to both development conditions. This is an interface contract,not implementation orresult. Private storage/construction details not explicitly fixed are implementation choices;listed operations andtypes mustbeusable.

## customer contract Public values, errors and identities

All Native declarations are in namespace autoeth except explicitly autoeth::e2e and autoeth::classic. Every byte view has call lifetime, permits nullptr only with size0, and requires a real accessible range of size bytes. No alignment or packed-struct overlay is required. A borrowed parameter is never retained by an asynchronous operation. A successful asynchronous admission owns the needed values, schemas, callback closure and immutable bytes before returning.

| Public value | Exact fields / meaning |
|---|---|
| ByteView | const uint8_t* data; size_t size |
| MutableByteView | uint8_t* data; size_t size |
| DurationNs / MonotonicNs | distinct wrappers around uint64_t value, nanoseconds; checked conversion/addition; no wall-clock reinterpretation |
| Header | uint16_t service_id,method_id,client_id,session_id;uint32_t length;uint8_t protocol_version,interface_version,message_type,return_code; host values, customer contract wire offsets |
| Status | Module module:uint16_t;uint32_t typed_code;uint64_t byte_offset;uint32_t detail;int32_t native_errno; no owning string or allocation |
| Module | Core1,Codec2,Tp3,E2e4,Sd5,Runtime6,Io7,Config8,Application9; source Std return/raw/generic/SM/wire/app domains remain separate |
| Errc | uint16_t Native local code values in the next paragraph; Status typed_code stores that value, module identifies origin; codec's richer CodecResult and raw E2E observations are separate records |
| ConfigDiagnostic | ConfigDetail code:uint16_t;std::string file,json_path;uint64_t byte_offset;bool path_complete;optional pointer in loader is call-lifetime/error-output,not retained;config out remains unchanged on failure |
| RuntimeHandle | std::unique_ptr<Runtime>; external owner keeps Runtime alive through stop/join; destruction from its callback/loop is outside the caller lifetime precondition |
| ServiceHandle,RequestHandle,SubscriptionHandle,CallHandle | distinct aggregate types, each slot:uint32_t,generation:uint64_t; a handle is local to its Runtime and operation family, never a wire ID or serializable cross-process capability |
| CompiledSchemaHandle | std::shared_ptr<const CompiledSchema>; immutable owned nodes/names/defaults after compilation |
| OfferSnapshot / EventSnapshot / CompletionSnapshot | small owning handles to immutable snapshot data; copies retain its real allocation budget token; callback const-reference is borrowed but its snapshot can be copied and retained |
| BuildInfo | project_name/project_version/standard_release are static-lifetime const char*;major/minor/patch:uint8_t;standard_major/minor/revision:uint8_t;vendor_id:uint16_t;vendor_id_assigned:bool;source manifest fingerprints belong to the release evidence,not invented runtime measurement |
| SecurityProvider | owning programmatic provider described by customer contract;virtual noexcept destructor;begin/cancel exact signatures and generation-qualified completion;no provider implementation comes from JSON |
| LayoutContext | uint64_t absolute_payload_origin_bytes; default_alignment_bits:uint16_t; caller uses actual full-frame origin16+E2EHeaderBytes |
| CompletionSnapshot data | Status status;ResultKind kind;bool output_present,checked;Value output;uint8_t wire_return_code,application_error;E2eObservation e2e;MonotonicNs completed_at;CallHandle call |
| E2eObservation | source call-return:uint8_t,raw-status:uint8_t,generic-status:uint8_t,SM-state:uint8_t;bool sm_enabled,metadata_valid;uint32_t received_counter,received_source_id; all fields are separately qualified, never a cast chain |
| ResultKind | Success,RemoteApplicationError,RemoteProtocolError,Timeout,Cancelled,LocalFailure; independent of whether an output Value exists |
| ServiceQuery | InterfaceId:uint32_t;service/instance:uint16_t,major:uint8_t,minor:uint32_t each with explicit Exact/Any discriminant;Transport enum;owned vector of CapabilityCriterion; no numeric sentinel inferred from a Native Exact value |
| CapabilityCriterion | owned key bytes,value bytes;ValuePresence={NoEquals,EmptyValue,Value}; ASCII-insensitive key comparison, exact value/kind; duplicate records retained |
| MethodRef / EventRef / FieldRef / EventGroupRef | own the selected OfferSnapshot or ServiceHandle plus schema-checked uint16 element/group identity; construction validates its declared role and exact service major |
| SubscriptionOptions | DurationNs retry_delay;uint32_t retry_max;bool retry_infinite;uint32_t ttl_seconds;Transport transport; selected endpoint; groups and flags validated before admission |
| RuntimeSnapshot | immutable owning collections of requested/current services, subscriptions, calls, connection generations, deadlines, payload-budget usage/high-water,as-of sequence/published_at,and best-effort diagnostic/lost records; no reference to mutable registry nodes |

Errc values: Ok0,NeedMore1,NullRange2,LengthTooSmall3,LengthOverflow4,OutputCapacity5,ProtocolVersion6,UnknownService7,InterfaceVersion8,UnknownMethod9,MessageType10,UnknownRequest11,PayloadMalformed12,InvalidConfig13,Resource14,Io15,Timeout16,Cancelled17,InvalidHandle18,AlreadyComplete19,InvalidState20,WouldDeadlock21,NoService22,UnsupportedProfile23,UnrepresentableClassicBitLength24,TransportAborted25,CleanupOverdue26,AssumptionViolation27,CounterExhausted28,HandlerFailure29,RemoteProtocol30,RemoteApplication31,NotReady32,VersionUnsupported33. These are local API values, not SOME/IP Return Codes. CodecResult retains customer contract's exact typed codes and node/DataID/offset; runtime maps it to local PayloadMalformed or InvalidConfig:Status.detail stores only CodecCode,and Status.byte_offset copies CodecResult.byteOffset. NodeId/DataID and the full result are not encoded in uint32 detail. The direct codec API returns the complete fixed-field CodecResult. A bounded best-effort diagnostic record may retain that full result,correlated with an operation/call when available;ring loss/overwrite has no full-retrieval guarantee. RemoteApplication carries the configured application code and separately the wire value.

A checked process-wide monotonic generation allocator prevents a handle from another live/destroyed Runtime accidentally matching a newly allocated slot with the same generation. It never wraps; exhaustion is CounterExhausted before admission/Protect. This Native identity allocator does not enter the pure E2E/codec libraries and does not promise cross-process handle reuse. Loop-owned transaction/transport state remains single-owner. SDK handle release never releases an old wire Session or a transport tombstone (customer contract).

## customer contract Exact Runtime signatures and callbacks

All following methods return Status and are noexcept unless an explicit different return is written. A failed immediate admission leaves its out handle unchanged and schedules no completion for work that was never admitted. Accepted calls have one eventual immutable terminal completion. Success from offer/request/subscribe/send means the stage specified in customer contract, not peer acceptance.

- Status create_runtime(const RuntimeConfig&,RuntimeHandle& out) noexcept;
- Status load_runtime_config(ByteView utf8,const std::string& base_directory,RuntimeConfig& out,ConfigDiagnostic* diagnostic=nullptr) noexcept;
- Status Runtime::start() noexcept;
- Status Runtime::offer_service(const ServiceRegistration&,ServiceHandle& out) noexcept;
- Status Runtime::set_available(ServiceHandle,bool available) noexcept;
- Status Runtime::request_service(const ServiceQuery&,AvailabilityCallback,RequestHandle& out) noexcept;
- Status Runtime::release_service(RequestHandle) noexcept;
- Status Runtime::call(const MethodRef&,const Value&,DurationNs,Completion,CallHandle& out) noexcept;
- Status Runtime::send_no_return(const MethodRef&,const Value&,SendCompletion) noexcept;
- Status Runtime::cancel(CallHandle) noexcept;
- Status Runtime::completion_snapshot(CallHandle,CompletionSnapshot& out) noexcept;
- Status Runtime::release_call_handle(CallHandle) noexcept;
- Status Runtime::subscribe(const EventGroupRef&,const SubscriptionOptions&,EventCallback,SubscriptionHandle& out) noexcept;
- Status Runtime::release_subscription(SubscriptionHandle) noexcept;
- Status Runtime::get_field(const FieldRef&,DurationNs,Completion,CallHandle& out) noexcept;
- Status Runtime::set_field(const FieldRef&,const Value&,DurationNs,Completion,CallHandle& out) noexcept;
- Status Runtime::publish_event(ServiceHandle,const EventRef&,const Value&,SendCompletion) noexcept;
- Status Runtime::update_field(ServiceHandle,const FieldRef&,const Value&,SendCompletion) noexcept;
- Status Runtime::snapshot(RuntimeSnapshot& out) noexcept;
- Status Runtime::request_stop(DurationNs drain) noexcept;
- Status Runtime::stop(DurationNs drain) noexcept;
- BuildInfo build_info() noexcept;

Callback aliases: Completion=std::function<void(const CompletionSnapshot&)>;SendCompletion=std::function<void(const SendSnapshot&)>;AvailabilityCallback=std::function<void(const AvailabilitySnapshot&)>;EventCallback=std::function<void(const EventSnapshot&)>;MethodHandler=std::function<MethodReply(const RequestContext&,const Value&)>;NoReturnHandler=std::function<void(const RequestContext&,const Value&)>.

ServiceRegistration owns immutable ServiceDescriptor and handler maps keyed by declared Method-ID. Its callbacks are complete for declared request/response and FF methods before offer admission; fields bind the declared getter/setter/notifier to the same state owner. RequestContext owns peer/interface/service/method/client/session/correlation and qualified E2E metadata; it does not expose mutable sockets or permit reusing an unqualified prior SourceID. MethodReply owns application_error:uint8_t and Value output; the code must be 0 or configured1..3F. A handler runs after current-message validation and before reply encoding. A thrown handler exception is caught at the runtime boundary, reported as HandlerFailure and follows the already-qualified protected generic-error path; no exception crosses C or the event-loop entry. FF/event/error inputs create no error loop. A malformed reply is a local serialization failure before reply Protect, not an excuse to emit unprotected bytes.

SendSnapshot owns Status and accepted/owned/kernel-progress counts, connection generation and completion time; local send completion is not an RPC result. AvailabilitySnapshot owns requested/current and the currently qualified offers. EventSnapshot owns its decoded Value and full E2eObservation, not a borrowed payload. A callback may enqueue later work or release its terminal call handle. It must not block/join its own loop or destroy its Runtime; request_stop is the nonblocking callback-safe operation. Callback exceptions are caught and diagnosed without a second business completion.

## customer contract Pure codec, TP and SD callable surfaces

customer contract Header/message operations return Status exactly as written there. customer contract schema/measure/encode/decode/normalize/export operations return CodecResult and retain its no-partial-output rules. Their exact signatures are present in the single TSV. The following complete the non-network stateful boundaries:

- Status segment(const Message&,const TpTxConfig&,SegmentSink&) noexcept;
- Status TpReceiver::accept(const Peer&,ByteView wire_frame,uint64_t now_ns,ReceiveOutcome& out) noexcept;
- void TpReceiver::expire(uint64_t now_ns,ExpirySink&) noexcept;
- void TpReceiver::reset(ResetReason) noexcept;
- Status decode_sd_message(ByteView,SdMessage& out) noexcept;
- Status encode_sd_message(const SdMessage&,MutableByteView,size_t& written) noexcept;
- Status SdEngine::dispatch(const SdEvent&,uint64_t now_ns,ActionBatch& out) noexcept;
- Status select_service(const ServiceQuery&,const std::vector<OfferSnapshot>&,RandomDraw&,OfferSnapshot& out) noexcept;

Message owns Header+payload bytes. TpTxConfig owns fixed FO/CP strategy, maximum segment payload, timeout and checked budgets; no mid-assembly strategy switch. Peer owns address/interface/transport/client identity. ReceiveOutcome owns zero/one completed Message plus explicit accepted/ignored/cancelled classification and diagnostic. A successful partial accept can change its receiver assembly while producing no completed message. A matched invalid segment may cancel according to customer contract; output atomicity does not roll back this permitted cancellation. SegmentSink takes ownership of each complete segment; its admission must reserve the full burst before any send/Protect side effect. ExpirySink receives each actual cancellation once. reset clears only that receiver's assemblies, invalidates timer generations and reports affected accepted work as specified; it does not stop unrelated runtimes.

SdEvent is a tagged owning value: Ingress,ServiceRequest,ServiceRelease,Availability,LinkIp,Connection,Timer. An ActionBatch is bounded owning output, with SendSD,RoutingChange,ConnectionRequest,CurrentStateChange,FieldInitial,Diagnostic variants. Its capacity is reserved before a state transition needing actions; resource failure cannot publish half a subscription/offer. customer contract governs source-specific ignore versus NACK and complete-message prevalidation. RandomDraw provides an exact unbiased integer in [0,bound); deterministic supplied draws are a test input, not production RNG assurance. Selector out is unchanged on failure; exact Instance bypasses all LB tiers. No DNS resolver or second CP SD engine is selected.

## customer contract Source C compatibility and host binding

The C catalogue preserves original names, qualifiers and ordered parameters, with only approved customer contract's exact three prototype changes. C99 fields follow their source tables; unnumbered SoAd enum values are explicitly LOCAL host assignments, never an original ABI claim. C++14 e2e.hpp exposes the separate Native wide P08/P08m and SmConfig signatures in customer contract. Native wrappers for other profiles consume their actual source C types and routines; they are not extra alias implementations counted as scale.

autoeth::classic::bind(Runtime&,const ClassicHostConfig&) noexcept and unbind(Runtime&) noexcept return Status. Bind is allowed only before start and before source module Init, for one explicitly selected Runtime per process; another live binding returns InvalidState. Unbind requires stopped/quiescent modules. The C99 consumer may be linked with a separate installed C++14 setup driver which creates this binding; this is a C99 call face, not an unsupported claim of a standalone full AUTOSAR ECU runtime. Source module Init preserves required ConfigPtr storage/lifetimes; Native configuration remains owned. Initialization order is PduR → SoAd → SomeIpTp → Sd. Runtime owns the actual common engine and calls selected scheduled functions; source scheduled entry points are nonreentrant and cannot be concurrently manually driven by another scheduler.

ClassicHostConfig owns the binding-to-Native identity maps and diagnostic adapter, but each original C ConfigPtr/array/callback remains live/read-only for its module initialization lifetime. An installed C99 translation unit constructs those exact structs and function pointer types; the setup driver does not rewrite them into private look-alikes. Source callbacks with borrowed PduInfo/meta are copied by the Native ingress consumer before returning, while synchronous PduR1:1 forwarding retains exact metadata and source BufReq results.

SoAd GetSoConMode and ReleaseRemoteAddr return void; IsConnectionReady returns TcpIp_ReturnType and takes a required RemoteAddrPtr. These signatures must not be simplified to bool or Std_ReturnType. Readiness maps actual native bind/link/IP/TCP/external-provider state to ready/pending/rejected; no ARP/physical-address or TLS handshake is fabricated. GetLocalAddr returns actual local address/prefix/default-router only if the host adapter obtained all requested facts; otherwise E_NOT_OK and all outputs unchanged. Caller address domains/capacities must match; no zero router is invented as success.

Selected C PDU-header mode uses the leading8B Message-ID/Length in SoAd; the upper SomeIpXf/SomeIpTp PDU starts at the8B Request-ID tail, with method E2E mapping as customer contract. A Native complete16B header is never placed behind another SoAd8B header. Header ID is each route's configured real Message-ID; length is recalculated from the actual upper PDU. Native endpoints always transmit complete SOME/IP framing. Classic header-disabled/TCP IF/ACK-dependent modes are not enabled by a Native configuration boolean.

The source-defined PDU routing and network state/error tables are customer contract. No full ECU gateway/ARXML/TcpIp ABI, security implementation, DET/DEM/IdsM stack or official certification is claimed. These boundaries do not excuse omitting the actual selected callbacks, copy/error/timing behavior or independent acceptance.

customer contract supplies the complete public RuntimeConfig/Value shape,external JSON,node references,role checks,source-specific channel configurations and operational file/capture semantics. Its actual configured profile/element state instances and added bounded UDP/membership/E2E registries are part of 06_配置字段与默认值.md resource accounting. Callback aliases SecurityCompletion=std::function<void(uint64_t,SecurityState)>;SecurityState={Pending,Ready,Failed,Expired};SecurityRequest owns endpoint/peer/connection generation. The actual provider is shared_ptr<SecurityProvider> stored by programmatic SecurityConfig bindings,name-matched to the JSON provider list beforestart.

ConfigDetail values are DuplicateKey1,UnknownKey2,Version3,Type4,Range5,Reference6,Cycle7,Role8,Unsupported9,Depth10,Count11,ProfileGeometry12,Context13,Io14. Loader validation order is UTF8/JSON syntax+duplicate keys → version → known fields/types/ranges in declared field order → references/cycles → roles/profile geometry → resource admission. Status detail carries ConfigDetail;the optional diagnostic intentionally receives failure file/path/offset. Diagnostic-text allocation failure reportsResource with path_complete=false rather than claiming a complete path. General Status remains allocation-free.


Runtime diagnostics are observably partial: each retained DiagnosticRecord has its sequence,time,optional qualified call/operation identity and optional complete CodecResult fields. Overflow overwrites the oldest diagnostic record and increments lostRecords;external owned snapshot copies remain immutable. Immediate rejected admissions cannot always be correlated to a retained record. RuntimeSnapshot is an as-of published immutable view with sequence/published_at,not a hard real-time freshness guarantee. Only direct codec API return values unconditionally carry their full fixed CodecResult. No status/ring loss changes business release or fabricates missing error details.

## customer contract Named Native-wide P08/P08m callable APIs

Header autoeth/e2e.hpp,namespace autoeth::e2e. 

P08ConfigWide and P08mConfigWide are distinct public types. Each has fields in this declared Native order: data_id:uint32_t;max_data_length_bits:uint64_t;max_delta_counter:uint32_t;min_data_length_bits:uint64_t;offset_bits:uint64_t. All three bit-geometry fields are uint64_t;data ByteView sizes are bytes. Do not pass these objects to original C config pointers. Pure-core state types below retain the customer contract selected source field/type layout.

| Exact public C++ operation | Contract |
|---|---|
| Std_ReturnType protect_p08(const P08ConfigWide&,E2E_P08ProtectStateType&,MutableByteView) noexcept | actual full buffer,checked size/config and P08 wire algorithm;no implicit Classic conversion |
| Std_ReturnType check_p08(const P08ConfigWide&,E2E_P08CheckStateType&,ByteView) noexcept | source P08 Check behavior;valid empty view is noData after required config/state conditions |
| Std_ReturnType protect_p08m(const P08mConfigWide&,E2E_P08mProtectStateType&,uint32_t source_id,Std_MessageTypeType,Std_MessageResultType,MutableByteView) noexcept | actual method metadata and20B header;real Counter mutation |
| Std_ReturnType source_check_p08m(const P08mConfigWide&,E2E_P08mCheckStateType&,uint32_t expected_source_id,Std_MessageTypeType,Std_MessageResultType,ByteView) noexcept | expectedSourceID by value;source comparison unchanged |
| Std_ReturnType sink_check_p08m(const P08mConfigWide&,E2E_P08mCheckStateType&,uint32_t& received_source_id,Std_MessageTypeType,Std_MessageResultType,ByteView) noexcept | required live output reference;post-integrity SourceID commit per supplied customer contract customer contract |
| Status to_classic_p08_config(const P08ConfigWide&,E2E_P08ConfigType& out) noexcept | all bit fields checked before uint32 narrowing;failure UNREPRESENTABLE_CLASSIC_BIT_LENGTH,out unchanged,no C call |
| Status to_classic_p08m_config(const P08mConfigWide&,E2E_P08mConfigType& out) noexcept | same separate method config conversion contract |

References are valid live objects;caller state/data lifetime and no concurrent mutation remain. Native byte views carry actual full lengths. Init/Map for the source state/status types use the original named routines;wide data processing invokes these Native operations and their actual common production algorithm. Native2^32-bit positive cases cannot use an unnamed C extension or a narrowed C wrapper. Original E2E_P08/P08m Config uint32 bit fields and C Protect/Check signatures remain unchanged,with separate real536870911-byte positives.

## customer contract Named Native state-machine configuration and bridge

Shared customer mapping version1 eventcustomer selection keeps original E2E_SMConfigType and C signatures unchanged. Native header autoeth/e2e.hpp exposes SmConfig with these exact fields: window_size_valid:uint8_t;min_ok_state_init:uint8_t;max_error_state_init:uint8_t;min_ok_state_valid:uint8_t;max_error_state_valid:uint8_t;min_ok_state_invalid:uint8_t;max_error_state_invalid:uint8_t;window_size_init:uint8_t;window_size_invalid:uint8_t;clear_from_valid_to_invalid:bool;transit_to_invalid_extended:bool;combined_no_data_init_count:bool. JSON/source-to-Native names map one-to-one;threshold/window rules remain customer contract.

Exact operations: Std_ReturnType sm_check(E2E_PCheckStatusType,const SmConfig&,E2E_SMCheckStateType&) noexcept;Std_ReturnType sm_check_init(E2E_SMCheckStateType&,const SmConfig&) noexcept;SmConfig from_classic_sm_config(const E2E_SMConfigType&) noexcept. State's live caller-owned status window is unchanged. from_classic maps ClearToInvalid to clear_from_valid_to_invalid and sets combined_no_data_init_count=false;Native callers explicitly select true/false. The C entry validates its pointers before this conversion. No field is added to the original C aggregate;other state/count/window/error conditions are not silently mixed. The supplied customer mapping is labeled supplied,not a measured method-only benefit.


The complete public ResourceConfig/SchedulingConfig/CorrelationConfig/CaptureConfig tables are in 06_配置字段与默认值.md. The fixed CodecResult/CodecCode andSchemaSpec/NodeSpec field/range contract is in 07_结果类型与Schema.md.


## Shared v1.3 configuration and result refinements

DetailedErrorSpec is an owning Native aggregate with std::uint8_t application_error=0 and std::string schema. The name must be nonempty and match an existing SchemaSpec.name. MethodConfig.detailed_errors is std::vector<DetailedErrorSpec>, default empty. Resolve names through the same RuntimeConfig; no new numeric ID allocation, lookup API or SchemaSpec field is added. The detailed-error schema uses existing role methodResponse. MethodReply and CompletionSnapshot fields/signatures are unchanged; 13_错误返回_UDP传输与通知.md fixes exact ERROR81 encoding, decoding and qualification.

ProvidedServiceConfig.notification_session_handling is an owning std::string, default INACTIVE, accepting exactly INACTIVE/ACTIVE. The current public contract defines its flow and original-frame commitment; it is separate from SD Session IDs and E2E counters.

All eight direct CodecResult functions retain their signatures and fourteen codes. The full approved table is 14_校验错误定位规则.md. Successful byteOffset/nodeId/dataId=0/0/0 is a no-error-location sentinel. Absolute coordinates and identifiable-node guarantees apply to error locations; Runtime narrow Status and lossy diagnostic snapshots do not gain this unconditional complete-result guarantee.

Struct identity/default presence is specified in 17_结构体成员身份与默认值.md; signed wire representation is specified in 16_有符号整数线上表示.md.


## v1.4 constructor conditions and unchanged interfaces

EndpointConfig.security_provider is owning std::string, defaultempty; exact name matching and the Native/JSON requires_security=false distinction are contractual. Bind the actual shared_ptr provider by the existing public API before start. No new provider API or C enum is introduced. Required SM roles need actual complete state_machine data, with no fabricated window/VALID state.

validate_message retains its signature and uses existing MessageType10 for supported-type/forbidden-ReturnCode combinations under the exact current order and reply qualification. Whole-root Array and Union boundary/NULL outcomes refine the existing decode_payload and direct CodecResult contract, without fields/signature changes.
