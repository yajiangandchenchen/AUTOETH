// Independent C++14 consumer of the installed AUTOETH Native surface.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "autoeth/build_info.hpp"
#include "autoeth/codec.hpp"
#include "autoeth/wire.hpp"

int main() {
  const autoeth::BuildInfo info = autoeth::build_info();
  autoeth::Header header;
  const std::vector<std::uint8_t> frame = {0x12, 0x34, 0x00, 0x01, 0x00, 0x00, 0x00,
                                           0x08, 0x46, 0x60, 0x00, 0x01, 0x01, 0x01,
                                           0x00, 0x00};
  std::size_t frame_bytes = 0;
  const autoeth::Status status = autoeth::decode_header(
      autoeth::ByteView(frame.data(), frame.size()), header, frame_bytes);
  autoeth::CodecResult result;
  result.code = autoeth::CodecCode::Ok;
  std::printf("cpp14 consumer %s %s service=0x%04x ok=%d codec_ok=%d\n", info.project_name,
              info.project_version, header.service_id, status.IsOk() ? 1 : 0,
              result.IsOk() ? 1 : 0);
  return status.IsOk() ? 0 : 1;
}