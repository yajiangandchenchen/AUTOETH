// AUTOETH Native build information. R0.5.1 / R0.5.3.
#ifndef AUTOETH_BUILD_INFO_HPP
#define AUTOETH_BUILD_INFO_HPP

#include <cstdint>

namespace autoeth {

struct BuildInfo {
  const char* project_name;
  const char* project_version;
  const char* standard_release;
  std::uint8_t major;
  std::uint8_t minor;
  std::uint8_t patch;
  std::uint8_t standard_major;
  std::uint8_t standard_minor;
  std::uint8_t standard_revision;
  std::uint16_t vendor_id;
  bool vendor_id_assigned;

  BuildInfo() noexcept
      : project_name(nullptr), project_version(nullptr), standard_release(nullptr), major(0u),
        minor(0u), patch(0u), standard_major(0u), standard_minor(0u), standard_revision(0u),
        vendor_id(0u), vendor_id_assigned(false) {}
};

BuildInfo build_info() noexcept;

}  // namespace autoeth

#endif