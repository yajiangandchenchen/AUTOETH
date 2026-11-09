# Shared customer increment v1.3: detailed errors, UDP and notification sessions

This increment applies the three Q018 choices approved on 2026-11-05. Preserve effective v1.2.1, all original specifications and all prior increments. These are supplied observable customer contracts for both conditions, not implementation, private oracle vectors or product results.

## Exact Native configuration types and schema identity

The precise Native configuration type is `struct DetailedErrorSpec { std::uint8_t application_error = 0; std::string schema; };`. Its owning `schema` string is a nonempty, exact `SchemaSpec.name`. The default object is invalid until both members are populated.

| Native declaration | Exact fields and default |
|---|---|
| `DetailedErrorSpec` | `std::uint8_t application_error = 0;` and owning `std::string schema;`. |
| `MethodConfig::detailed_errors` | `std::vector<DetailedErrorSpec>`, default empty. This adds the detailed-error mapping to the existing method configuration. |
| `ProvidedServiceConfig::notification_session_handling` | Owning `std::string`, default `"INACTIVE"`. Only `"INACTIVE"` and `"ACTIVE"` are valid. No new public enum is required. |

The JSON field is `detailed_errors:[{application_error:integer,schema:string}]`. Both JSON and programmatic Native configuration use the same schema-name reference as ordinary request/response schemas. Resolve it through the names in that RuntimeConfig before registration. Reject an empty/unknown name or a numeric JSON schema value. The earlier logical term SchemaId describes schema identity; this configuration member is explicitly a name string. No new numeric ID assignment rule, public lookup API or SchemaSpec field is introduced.

A detailed-error schema uses the existing `role=methodResponse`; no new schema-role enum is added. Its `argumentSet`, nodes, lengths, byte order, alignment, values and defaults follow the existing response-payload rules. Both plain and tagged response schemas remain available under their existing contracts.

## Detailed application errors

Each `application_error` is in `1..63` and must also occur in the method's `application_errors`. The vector has at most one entry per code. Reject duplicates, invalid code, unresolved schema, wrong schema role and a detailed-error declaration on a fire-and-forget method during configuration validation. Preserve the old configuration/registration and publish no availability first. An empty mapping preserves existing behavior.

When a handler returns a configured nonzero code:

| Method configuration | Wire and Value behavior |
|---|---|
| No `detailed_errors` entry for the code | Existing RESPONSE `0x80`; wire return code is application code plus `0x1F`; output uses the ordinary response schema. |
| Matching `detailed_errors` entry | ERROR `0x81`; the same return-code mapping; the original `MethodReply.output` is encoded with that entry's schema. |
| Code zero | Existing successful RESPONSE `0x80`; ordinary response schema. |
| Undeclared code or invalid output Value | Existing local configuration/value/serialization error, before reply Protect; no unprotected fallback or invented successful frame. |

`MethodReply` keeps its existing `application_error` and owning `Value output`. `CompletionSnapshot` keeps its existing fields. A matching valid Native detailed ERROR is `ResultKind::RemoteApplicationError`; its original wire code and inverse application code remain separate, and its decoded Value is committed to `output` with `output_present=true`. A decode/capacity failure does not commit a partial Value or create another error response. Existing local codec/status diagnostics and failure atomicity apply.

For a protected method, only current trusted request metadata qualifies an error reply. Form a real protected ERROR `0x81`, using the existing canonical response/error interpretation, the actual transaction counter and qualified SourceID. Validate and reserve before Protect. No unprotected fallback is permitted. Completed receive Check/SM effects are not rolled back by a later payload failure.

Automatic protocol/E2E protected RESPONSE `0x80` errors retain their existing contract. They are not converted to detailed application ERRORs. Classic's existing treatment of ERROR `0x81` ignores its application payload; the raw codec still owns/displays bounded payload bytes. The Native typed capability does not assert an extra Classic detailed-error ABI.

## UDP capacity and system IP fragmentation

The effective configured UDP datagram capacity is `max_udp_datagram_bytes`, checked against the existing family maximum: 65507 for IPv4 or 65527 for IPv6 without jumbograms. The default remains 1472 and the minimum remains 16. Observe actual interface and path properties and report real errors, but do not unconditionally reduce this configured capacity to interface MTU minus headers.

A legal capacity may exceed the interface MTU. The Runtime must allow system IP fragmentation rather than actively prohibiting it. This does not guarantee successful routing, fragment delivery or peer receipt. `EMSGSIZE` and other actual I/O failures remain observable. This contract does not authorize changing the host's MTU, routes, proxy or global network settings.

TP remains explicitly selected. A frame that exceeds the configured datagram capacity follows the selected TP policy or fails before Protect/admission where no TP route is selected. A frame larger than the observed interface MTU but within configured capacity is not rejected solely for that MTU difference.

For a configured TP route, the nonfinal payload upper bound is:

`floor(min(configuredMaxSegmentPayload, effectiveDatagramBytes - 20) / 16) * 16`

Use checked subtraction. It must permit at least sixteen bytes. This calculation uses the actual configured/family-limited datagram capacity, not an unconditional interface-MTU cap.

Keep evidence separate: a real larger-than-interface-MTU API attempt, actual observed IP fragmentation/reassembly on a qualified network fixture, and controlled I/O failure are different observations. A large localhost datagram alone does not prove IP fragmentation.

## Notification Session Handling

The provided service JSON field `notification_session_handling` accepts `INACTIVE` and `ACTIVE`, default `INACTIVE`. Requested-service declarations do not select the remote server's mode. Runtime notifications always use Client ID zero. Raw codec calls retain their explicitly supplied valid header values.

| Mode | Session behavior |
|---|---|
| INACTIVE | Session ID zero for the notification. |
| ACTIVE | A separate nextSession starts at one and uses `1..0xFFFF`; after `0xFFFF`, wrap to one. |

An ACTIVE notification flow is qualified by interface, service, instance, major version, event ID, transport and the real unicast peer or multicast destination. Initial and regular notifications with distinct actual E2E/transport flows have independent state. Different flows do not consume each other's nextSession.

After validation and resource reservation, preallocate the Session ID for a new complete original frame before Protect. Advance only when that owned original frame is successfully committed. Failure before commitment does not advance it. Cancellation or I/O failure after commitment does not roll back it or an already consumed E2E counter.

EAGAIN retries the same owned frame and ID. All TP child segments preserve the original Session ID; do not increment per segment. A legally shared immutable frame of one actual flow allocates only once. Different target flows are independent. These are explicit LOCAL flow and commitment interpretations of transmission, not a claim that queue commitment is a kernel-send or peer-receipt fact.

Session handling is distinct from SD Session IDs and E2E counters. The receive side gains no unsupported infinite replay guarantee or notification-to-RPC correlation.

## Ordinary examples and common identity

The supplied detailed-error schema file contains the request, normal response and detailed error response schemas. The server/client configurations use the same service/instance/method/version and exact symbolic schema names. The error Value file is ordinary application data, not an independent golden-wire expectation.

The independent Struct member-identity and HALF_CYCLE/RRD timing increments are incorporated as common requirements as well. Identify effective v1.3 and those supplied choices in the first complete candidate and later regressions. Freezing or distributing this input does not approve the method's document gate or static generation.


The complete current min-before-alignment rule, final-tail limits and Classic local-header unit mapping are in 19_TP负载上限与对齐.md. Role-SM construction and exact provider names are in 21_状态机角色与安全提供者名称.md.
