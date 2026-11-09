// AUTOETH classic host binding (S9 groundwork). R9.1: bind is allowed only
// before start and only once per process; unbind requires a quiescent runtime.
#include "autoeth/classic_host.hpp"

namespace autoeth {
namespace classic {

DiagnosticObserver::DiagnosticObserver() noexcept = default;

DiagnosticObserver::~DiagnosticObserver() noexcept = default;

void DiagnosticObserver::OnDiagnostic(const ClassicDiagnostic& diagnostic) noexcept {
  (void)diagnostic;
}

namespace {

struct BindingState {
  bool bound;
  std::uint32_t bound_runtime_tag;
  std::uint32_t rejected_calls;

  BindingState() noexcept : bound(false), bound_runtime_tag(0u), rejected_calls(0u) {}
};

BindingState g_binding;

bool ValidateConfig(const ClassicHostConfig& config) noexcept {
  // R9.4: every aggregate count must match its pointer pair as a whole before
  // Init publishes initialized state.
  if (config.pdur != nullptr) {
    if (config.pdur->pathCount != 0u && config.pdur->paths == nullptr) {
      return false;
    }
    if (config.pdur->groupCount != 0u && config.pdur->groups == nullptr) {
      return false;
    }
  }
  if (config.soad != nullptr) {
    if (config.soad->schemaVersion != 1u) {
      return false;
    }
    if (config.soad->soConCount != 0u && config.soad->soCons == nullptr) {
      return false;
    }
    if (config.soad->txRouteCount != 0u && config.soad->txRoutes == nullptr) {
      return false;
    }
    if (config.soad->rxRouteCount != 0u && config.soad->rxRoutes == nullptr) {
      return false;
    }
    if (config.soad->routingGroupCount != 0u && config.soad->routingGroups == nullptr) {
      return false;
    }
  }
  if (config.someip_tp != nullptr) {
    if (config.someip_tp->txPduCount != 0u && config.someip_tp->txPdus == nullptr) {
      return false;
    }
    if (config.someip_tp->rxPduCount != 0u && config.someip_tp->rxPdus == nullptr) {
      return false;
    }
  }
  if (config.sd != nullptr) {
    if (config.sd->serverCount != 0u && config.sd->servers == nullptr) {
      return false;
    }
    if (config.sd->clientCount != 0u && config.sd->clients == nullptr) {
      return false;
    }
    if (config.sd->consumedGroupCount != 0u && config.sd->consumedGroups == nullptr) {
      return false;
    }
    if (config.sd->serviceGroupCount != 0u && config.sd->serviceGroups == nullptr) {
      return false;
    }
  }
  return true;
}

}  // namespace

Status bind(Runtime& runtime, const ClassicHostConfig& config) noexcept {
  if (g_binding.bound) {
    ++g_binding.rejected_calls;
    return Status::Error(ModuleRuntime, InvalidState);
  }
  if (!ValidateConfig(config)) {
    return Status::Error(ModuleConfig, InvalidConfig);
  }
  // Initialization order is PduR, SoAd, SomeIpTp, Sd (R9.2).
  if (config.pdur != nullptr) {
    PduR_Init(config.pdur);
  }
  if (config.soad != nullptr) {
    SoAd_Init(config.soad);
  }
  if (config.someip_tp != nullptr) {
    SomeIpTp_Init(config.someip_tp);
  }
  if (config.sd != nullptr) {
    Sd_Init(config.sd);
  }
  if (config.someip_xf != nullptr) {
    SomeIpXf_Init(config.someip_xf);
  }
  if (config.e2e_xf != nullptr) {
    E2EXf_Init(config.e2e_xf);
  }
  g_binding.bound = true;
  g_binding.bound_runtime_tag = 1u;
  (void)runtime;
  return Status::Success();
}

Status unbind(Runtime& runtime) noexcept {
  if (!g_binding.bound) {
    return Status::Error(ModuleRuntime, InvalidState);
  }
  SomeIpXf_DeInit();
  E2EXf_DeInit();
  g_binding.bound = false;
  g_binding.bound_runtime_tag = 0u;
  (void)runtime;
  return Status::Success();
}

}  // namespace classic
}  // namespace autoeth