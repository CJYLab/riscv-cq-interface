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

#include "./fix16.h"

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

fix16_t affine (const int output_size,
                const int input_size,
                const int batch_size,
                fix16_t *out,           // [batch_size][output_size],
                const fix16_t *in_data, // [batch_size][input_size],
                const fix16_t *wh,      // [input_size][output_size],
                const fix16_t *wb);      // [output_size]

void relu (const int batch_size,
		   const int size,
		   fix16_t *o,         // [batch_size][size],
		   const fix16_t *e);  // [batch_size][size]

fix16_t softmax (const int batch_size,
                 const int size,
                 fix16_t       *o,  // [batch_size][size],
                 const fix16_t *e); // [batch_size][size]

void TestNetwork (const int num_images,
				  const int input_size,
				  const int output_size,
				  const int hidden_size,
				  const fix16_t *wh0,   // [input_size][hidden_size],
				  const fix16_t *wb0,   // [hidden_size],
				  const fix16_t *wh1,   // [hidden_size][output_size],
				  const fix16_t *wb1);   // [output_size]

int argmax (const int x_size, fix16_t *o);

extern char _binary_t10k_images_idx3_ubyte_start[];
extern char _binary_t10k_images_idx3_ubyte_end[];

extern char _binary_t10k_labels_idx1_ubyte_start[];
extern char _binary_t10k_labels_idx1_ubyte_end[];

extern char _binary_wb0_bin_start[];
extern char _binary_wb0_bin_end[];
extern char _binary_wb1_bin_start[];
extern char _binary_wb1_bin_end[];
extern char _binary_wh0_bin_start[];
extern char _binary_wh0_bin_end[];
extern char _binary_wh1_bin_start[];
extern char _binary_wh1_bin_end[];


const fix16_t *wh0 = (fix16_t *)_binary_wh0_bin_start;  // [INPUTNO * HIDDENNO];
const fix16_t *wb0 = (fix16_t *)_binary_wb0_bin_start;  // [HIDDENNO];
const fix16_t *wh1 = (fix16_t *)_binary_wh1_bin_start;  // [HIDDENNO * OUTPUTNO];
const fix16_t *wb1 = (fix16_t *)_binary_wb1_bin_start;  // [OUTPUTNO];          

int main ()
{
  TestNetwork ( 10, INPUTNO, OUTPUTNO, HIDDENNO, wh0, wb0, wh1, wb1);
  TestNetwork ( 20, INPUTNO, OUTPUTNO, HIDDENNO, wh0, wb0, wh1, wb1);
  TestNetwork ( 50, INPUTNO, OUTPUTNO, HIDDENNO, wh0, wb0, wh1, wb1);
  TestNetwork (100, INPUTNO, OUTPUTNO, HIDDENNO, wh0, wb0, wh1, wb1);
  TestNetwork (200, INPUTNO, OUTPUTNO, HIDDENNO, wh0, wb0, wh1, wb1);

  return 0;
}

fix16_t af0 [BATCH_SIZE * HIDDENNO];
fix16_t fix16_in_data[BATCH_SIZE*INPUTNO];
char *in_data;
char *ans_data;
fix16_t af1 [BATCH_SIZE * OUTPUTNO];
fix16_t rel0[BATCH_SIZE * HIDDENNO];

void TestNetwork (const int num_images,
				  const int input_size,
				  const int output_size,
				  const int hidden_size,
				  const fix16_t *wh0,  // [input_size][hidden_size],
				  const fix16_t *wb0,  // [hidden_size],
				  const fix16_t *wh1,  // [hidden_size][output_size],
				  const fix16_t *wb1)  // [output_size]
{
  const char *message0 = "=== TestNetwork ===\n";
  write (STDOUT_FILENO, message0, strlen (message0));

  in_data  = &_binary_t10k_images_idx3_ubyte_start[0x10];
  ans_data = &_binary_t10k_labels_idx1_ubyte_start[0x08];

  int correct = 0;
  uint64_t start_cycle, stop_cycle;
  rdmcycle(&start_cycle);
  
  for (int no_input = 0; no_input < num_images * BATCH_SIZE; no_input += BATCH_SIZE) {
	for (int i = 0; i < 28 * 28 * BATCH_SIZE; i++) {
	  fix16_in_data[i] = (in_data[i] << 8);
	  /*
		if ((i % 28) == 27) { write (STDOUT_FILENO, "\r\n", 2); }
	  */
	}

	affine (HIDDENNO, INPUTNO,  BATCH_SIZE, af0, (const fix16_t *)fix16_in_data, wh0, wb0);
	relu (BATCH_SIZE, HIDDENNO, rel0, af0);
	affine (OUTPUTNO, HIDDENNO, BATCH_SIZE, af1, rel0,    wh1, wb1);

	for (int b = 0; b < BATCH_SIZE; b++) {
	  int t = argmax (OUTPUTNO, &af1[b * OUTPUTNO]);
	  if (t == ans_data[b]) correct++;
	}

	in_data += (INPUTNO * BATCH_SIZE);
	ans_data += BATCH_SIZE;
  }

  rdmcycle(&stop_cycle);
  uint64_t proc_cycle = stop_cycle - start_cycle;
  
  printf ("Correct = %d\n", correct);
  printf ("Time = %08x%08x - %08x%08x = %08x%08x\n", (uint32_t)(stop_cycle >> 32), (uint32_t)(stop_cycle & 0x0ffffffff),
		  (uint32_t)(start_cycle >> 32), (uint32_t)(start_cycle & 0x0ffffffff),
		  (uint32_t)(proc_cycle >> 32), (uint32_t)(proc_cycle & 0x0ffffffff));

  return;
}



fix16_t affine (const int output_size,
			   const int input_size,
			   const int batch_size,
			   fix16_t *out,            // [batch_size][output_size],
			   const fix16_t *in_data,  // [batch_size][input_size],
			   const fix16_t *wh,       // [input_size][output_size],
			   const fix16_t *wb)       // [output_size]
{
  for (int b = 0; b < batch_size; b++) {
  	for (int o = 0; o < output_size; o++) {
  	  out[b * output_size + o] = 0;
  	  for (int i = 0; i < input_size; i++) {
  	  	out[b * output_size + o] = fix16_add (out[b * output_size + o],
                                              fix16_mul (in_data[b * input_size + i], wh[i * output_size + o]));
  	  }
  	  out[b * output_size + o] = fix16_add (out[b * output_size + o], wb[o]);
  	}
  }
}


void relu (const int batch_size,
		   const int size,
		   fix16_t *o,        // [batch_size][size],
		   const fix16_t *e)  // [batch_size][size]
{
  for (int b = 0; b < batch_size; b++) {
	for (int i = 0; i < size; i++) {
	  o[b * size + i] = e[b * size + i] > 0 ? e[b * size + i] : 0;
	}
  }
  return;
}


fix16_t softmax (const int batch_size,
                 const int size,
                 fix16_t *o,       // [batch_size][size],
                 const fix16_t *e) // e[batch_size][size]
{
  fix16_t *max = (fix16_t *)malloc(sizeof(fix16_t) * batch_size);
  for (int b = 0; b < batch_size; b++) {
	max[b] = e[b * size + 0];
	for (int i = 1; i < size; i++) {
	  max[b] = max[b] < e[b * size + i] ? e[b * size + i] : max[b];
	}
  }

  for (int b = 0; b < batch_size; b++) {
	fix16_t exp_sum;
    exp_sum = 0;
	fix16_t *a = (fix16_t *)malloc(sizeof(fix16_t) * size);
	for (int i = 0; i < size; i++) {
      a[i] = fix16_sub (e[b * size + i], max[b]);
	  exp_sum = fix16_add (exp_sum, fix16_exp(a[i]));
	}
	for (int i = 0; i < size; i++) {
	  o[b * size + i] = fix16_div (fix16_exp(a[i]), exp_sum);
	}
    free (a);
  }


  free (max);
}


int argmax (const int x_size, fix16_t *o)
{
  fix16_t ret = o[0];
  int    max_idx = 0;
  for (int i = 1; i < x_size; i++) {
	if (o[i] > ret) {
	  ret = o[i];
	  max_idx = i;
	}
  }

  return max_idx;
}
