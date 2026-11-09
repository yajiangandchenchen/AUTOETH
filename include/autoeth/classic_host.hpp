// AUTOETH Classic host binding surface. R9.1 / R9.3.
#ifndef AUTOETH_CLASSIC_HOST_HPP
#define AUTOETH_CLASSIC_HOST_HPP

#include <cstdint>

#include "autoeth/classic/E2E.h"
#include "autoeth/classic/E2EXf.h"
#include "autoeth/classic/PduR.h"
#include "autoeth/classic/Sd.h"
#include "autoeth/classic/SoAd.h"
#include "autoeth/classic/SomeIpTp.h"
#include "autoeth/classic/SomeIpXf.h"
#include "autoeth/runtime.hpp"

namespace autoeth {
namespace classic {

struct ClassicDiagnostic {
  std::uint16_t module_id;
  std::uint16_t api_id;
  std::uint8_t error_id;
  bool development;
};

class DiagnosticObserver {
 public:
  DiagnosticObserver() noexcept;
  virtual ~DiagnosticObserver() noexcept;
  DiagnosticObserver(const DiagnosticObserver&) = delete;
  DiagnosticObserver& operator=(const DiagnosticObserver&) = delete;

  virtual void OnDiagnostic(const ClassicDiagnostic& diagnostic) noexcept;
};

struct ClassicHostConfig {
  const PduR_PBConfigType* pdur;
  const SoAd_ConfigType* soad;
  const SomeIpTp_ConfigType* someip_tp;
  const Sd_ConfigType* sd;
  const SomeIpXf_ConfigType* someip_xf;
  const E2EXf_ConfigType* e2e_xf;
  DiagnosticObserver* observer;

  ClassicHostConfig() noexcept
      : pdur(nullptr), soad(nullptr), someip_tp(nullptr), sd(nullptr), someip_xf(nullptr),
        e2e_xf(nullptr), observer(nullptr) {}
};

Status bind(Runtime& runtime, const ClassicHostConfig& config) noexcept;
Status unbind(Runtime& runtime) noexcept;

}  // namespace classic
}  // namespace autoeth

#endif