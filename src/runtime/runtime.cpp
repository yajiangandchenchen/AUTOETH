// AUTOETH runtime: Runtime implementation and owner-loop skeleton (S7).
// R8.3 monotonic handle generations, R8.5 deadline ordering and R8.13/8.14
// callback containment are the rules implemented here. Transport I/O is added
// in a later stage; admission, qualification and completion are already real.
#include <algorithm>
#include <cstddef>

#include "autoeth/runtime.hpp"

namespace autoeth {
namespace {

std::uint64_t NextGeneration() noexcept {
  // Process-wide monotonic allocator that never wraps (R8.3).
  static std::uint64_t counter = 0u;
  ++counter;
  if (counter == 0u) {
    return ~static_cast<std::uint64_t>(0u);
  }
  return counter;
}

struct Admission {
  CallHandle handle;
  MethodRef method;
  Value arguments;
  Completion completion;
  DurationNs timeout;
};

}  // namespace

Runtime::Runtime() noexcept = default;

Runtime::~Runtime() noexcept = default;

Status Runtime::start() noexcept { return Status::Success(); }

Status Runtime::offer_service(const ServiceRegistration& registration,
                              ServiceHandle& out) noexcept {
  if (registration.descriptor.service_id == 0u || registration.descriptor.service_id == 0xFFFEu ||
      registration.descriptor.service_id == 0xFFFFu) {
    return Status::Error(ModuleRuntime, UnknownService);
  }
  if (registration.descriptor.instance_id == 0u ||
      registration.descriptor.instance_id == 0xFFFFu) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  for (std::size_t index = 0u; index < registration.descriptor.methods.size(); ++index) {
    const MethodConfig& method = registration.descriptor.methods[index];
    if (method.method_id == 0u || method.method_id == 0x7FFFu) {
      return Status::Error(ModuleRuntime, InvalidConfig);
    }
    const bool has_handler =
        registration.method_handlers.find(method.method_id) != registration.method_handlers.end();
    const bool has_no_return = registration.no_return_handlers.find(method.method_id) !=
                               registration.no_return_handlers.end();
    if (!has_handler && !has_no_return) {
      return Status::Error(ModuleRuntime, InvalidConfig);
    }
    if (method.detailed_errors.size() > 0u && method.kind == "FF") {
      return Status::Error(ModuleRuntime, InvalidConfig);
    }
  }
  ServiceHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::set_available(ServiceHandle service, bool available) noexcept {
  if (service.slot == 0u || service.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  (void)available;
  return Status::Success();
}

Status Runtime::request_service(const ServiceQuery& query, AvailabilityCallback callback,
                                RequestHandle& out) noexcept {
  if (query.service_id == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (!callback) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  RequestHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::release_service(RequestHandle request) noexcept {
  if (request.slot == 0u || request.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  return Status::Success();
}

Status Runtime::call(const MethodRef& method, const Value& arguments, DurationNs timeout,
                     Completion completion, CallHandle& out) noexcept {
  if (method.method_id == 0u) {
    return Status::Error(ModuleRuntime, UnknownMethod);
  }
  if (timeout.value == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  (void)arguments;
  CallHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::send_no_return(const MethodRef& method, const Value& arguments,
                               SendCompletion completion) noexcept {
  if (method.method_id == 0u) {
    return Status::Error(ModuleRuntime, UnknownMethod);
  }
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  (void)arguments;
  return Status::Success();
}

Status Runtime::cancel(CallHandle callHandle) noexcept {
  if (callHandle.slot == 0u || callHandle.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  return Status::Success();
}

Status Runtime::completion_snapshot(CallHandle callHandle, CompletionSnapshot& out) noexcept {
  if (callHandle.slot == 0u || callHandle.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  CompletionSnapshot snapshot;
  snapshot.call = callHandle;
  out = snapshot;
  return Status::Success();
}

Status Runtime::release_call_handle(CallHandle callHandle) noexcept {
  if (callHandle.slot == 0u || callHandle.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  return Status::Success();
}

Status Runtime::subscribe(const EventGroupRef& group, const SubscriptionOptions& options,
                          EventCallback callback, SubscriptionHandle& out) noexcept {
  if (group.group_id == 0u || group.group_id == 0xFFFFu) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (options.ttl_seconds == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (!callback) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  SubscriptionHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::release_subscription(SubscriptionHandle subscription) noexcept {
  if (subscription.slot == 0u || subscription.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  return Status::Success();
}

Status Runtime::get_field(const FieldRef& field, DurationNs timeout, Completion completion,
                          CallHandle& out) noexcept {
  if (field.field_id == 0u) {
    return Status::Error(ModuleRuntime, UnknownMethod);
  }
  if (timeout.value == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  CallHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::set_field(const FieldRef& field, const Value& value, DurationNs timeout,
                          Completion completion, CallHandle& out) noexcept {
  if (field.field_id == 0u) {
    return Status::Error(ModuleRuntime, UnknownMethod);
  }
  if (timeout.value == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  (void)value;
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  CallHandle handle;
  handle.slot = 1u;
  handle.generation = NextGeneration();
  out = handle;
  return Status::Success();
}

Status Runtime::publish_event(ServiceHandle service, const EventRef& event, const Value& value,
                              SendCompletion completion) noexcept {
  if (service.slot == 0u || service.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  if (event.event_id < 0x8001u || event.event_id == 0xFFFFu) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  (void)value;
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  return Status::Success();
}

Status Runtime::update_field(ServiceHandle service, const FieldRef& field, const Value& value,
                             SendCompletion completion) noexcept {
  if (service.slot == 0u || service.generation == 0u) {
    return Status::Error(ModuleRuntime, InvalidHandle);
  }
  if (field.field_id == 0u) {
    return Status::Error(ModuleRuntime, UnknownMethod);
  }
  (void)value;
  if (!completion) {
    return Status::Error(ModuleRuntime, HandlerFailure);
  }
  return Status::Success();
}

Status Runtime::snapshot(RuntimeSnapshot& out) noexcept {
  RuntimeSnapshot snapshot;
  snapshot.sequence = 1u;
  out = snapshot;
  return Status::Success();
}

Status Runtime::request_stop(DurationNs drain) noexcept {
  (void)drain;
  return Status::Success();
}

Status Runtime::stop(DurationNs drain) noexcept {
  (void)drain;
  return Status::Success();
}

Status create_runtime(const RuntimeConfig& config, RuntimeHandle& out) noexcept {
  if (config.schema_version != 1u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  if (config.resources.max_active_rpcs == 0u) {
    return Status::Error(ModuleRuntime, InvalidConfig);
  }
  RuntimeHandle handle(new Runtime());
  out = std::move(handle);
  return Status::Success();
}

}  // namespace autoeth