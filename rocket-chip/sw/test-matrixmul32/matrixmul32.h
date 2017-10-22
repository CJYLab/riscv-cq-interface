// See LICENSE for license details.

#ifndef SRC_MAIN_C_MATRIXMUL32_H
#define SRC_MAIN_C_MATRIXMUL32_H

#include "rocc.h"

#define XCUSTOM_MATRIXMUL32 0

#define k_MTRXMUL_SETM   (0)
#define k_MTRXMUL_SETK   (1)
#define k_MTRXMUL_DOCALC (2)

#define matrixmul32_setM(y, len)                                    \
  ROCC_INSTRUCTION(XCUSTOM_MATRIXMUL32, y, len, 0, k_MTRXMUL_SETM);
#define matrixmul32_setK(y, len)                                    \
  ROCC_INSTRUCTION(XCUSTOM_MATRIXMUL32, y, len, 0, k_MTRXMUL_SETK);
#define matrixmul32(y, mem_addr0, mem_addr1)                            \
  ROCC_INSTRUCTION(XCUSTOM_MATRIXMUL32, y, mem_addr0, mem_addr1, k_MTRXMUL_DOCALC);

#endif  // SRC_MAIN_C_MATRIXMUL32_H
