// AUTOETH core: classic module version helpers shared by C adapters.
#include <cstdint>

namespace autoeth {

void FillVersionInfo(std::uint16_t module_id, std::uint8_t major, std::uint8_t minor,
                     std::uint8_t patch, std::uint16_t* vendor, std::uint16_t* module,
                     std::uint8_t* out_major, std::uint8_t* out_minor, std::uint8_t* out_patch) {
  if (vendor != nullptr) {
    *vendor = 0x0000u;
  }
  if (module != nullptr) {
    *module = module_id;
  }
  if (out_major != nullptr) {
    *out_major = major;
  }
  if (out_minor != nullptr) {
    *out_minor = minor;
  }
  if (out_patch != nullptr) {
    *out_patch = patch;
  }
}

}  // namespace autoeth