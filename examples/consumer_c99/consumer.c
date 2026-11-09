/* Independent C99 consumer of the installed AUTOETH Classic C surface.
 * It includes only installed public headers and uses C99 dialect features. */
#include <stdio.h>

#include "autoeth/classic/Crc.h"
#include "autoeth/classic/E2E.h"
#include "autoeth/classic/Std_Types.h"

int main(void) {
  const uint8 data[8] = {0x00u, 0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u, 0x77u};
  const uint8 crc8 = Crc_CalculateCRC8(data, 8u, 0u, TRUE);
  uint8 result = 0u;
  E2E_P22ProtectStateType protect_state;
  Std_ReturnType status = E2E_E_OK;

  status = E2E_P22ProtectInit(&protect_state);
  if (status != E2E_E_OK) {
    return 1;
  }
  printf("c99 consumer crc8=0x%02x counter=%u\n", (unsigned)crc8,
         (unsigned)protect_state.Counter);
  result = (uint8)(crc8 ^ 0x01u);
  return (result == 0u) ? 1 : 0;
}