// =========================================
//
// train_twolayernet_fix16.c
//
// =========================================

#define IMAGE_FILE "train-images-idx3-ubyte"
#define LABEL_FILE "train-labels-idx1-ubyte"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef GPERF
#include <gperftools/profiler.h>
#endif // GPERF

#define INPUTNO  (28*28)    // No of input cell
#define OUTPUTNO (10)
#define HIDDENNO (50)    // No of hidden cell
#define ALPHA    (10)   // Coefficient of learning
#define SEED     (65535)  // Seed of random
#define MAXINPUTNO (60000)  // Max number of learning data
#define BATCH_SIZE (2)
#define LEARNING_RATE (0.1)
#define WEIGHT_INIT (0.01)

#define rdmcycle(x)  {								   \
    uint32_t lo, hi, hi2;                              \
    __asm__ __volatile__ ("1:\n\t"                     \
                          "csrr %0, mcycleh\n\t"       \
                          "csrr %1, mcycle\n\t"        \
                          "csrr %2, mcycleh\n\t"       \
                          "bne  %0, %2, 1b\n\t"                 \
                          : "=r" (hi), "=r" (lo), "=r" (hi2)) ; \
	*(x) = lo | ((uint64_t) hi << 32);							\
  }

extern char _binary_t10k_images_idx3_ubyte_start[];
extern char _binary_t10k_images_idx3_ubyte_end[];

extern char _binary_t10k_labels_idx1_ubyte_start[];
extern char _binary_t10k_labels_idx1_ubyte_end[];

int main ()
{
  const char *message0 = "=== TestNetwork ===\n";
  write (STDOUT_FILENO, message0, strlen (message0));

  char *in_data  = &_binary_t10k_images_idx3_ubyte_start[0x10];

  for (int no_images = 0; no_images < 100; no_images++) {
	for (int i = 0; i < 28 * 28; i++) {
	  printf ("%4d", in_data[i]);
	  if ((i % 28) == 27) { printf ("\r\n"); }
	}
	in_data += INPUTNO;
  }
  return 0;
}

