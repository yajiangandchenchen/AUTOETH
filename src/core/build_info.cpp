// AUTOETH core: build information. R0.5.1 / R0.5.3.
#include "autoeth/build_info.hpp"

namespace autoeth {
namespace core_detail {

const char kProjectName[] = "autoeth";
const char kProjectVersion[] = "0.1.0";
const char kStandardRelease[] = "R24-11";

}  // namespace core_detail
using core_detail::kProjectName;
using core_detail::kProjectVersion;
using core_detail::kStandardRelease;

BuildInfo build_info() noexcept {
  BuildInfo info;
  info.project_name = kProjectName;
  info.project_version = kProjectVersion;
  info.standard_release = kStandardRelease;
  info.major = 0u;
  info.minor = 1u;
  info.patch = 0u;
  info.standard_major = 0x04u;
  info.standard_minor = 0x0Au;
  info.standard_revision = 0x00u;
  info.vendor_id = 0x0000u;
  info.vendor_id_assigned = false;
  return info;
}

}  // namespace autoeth