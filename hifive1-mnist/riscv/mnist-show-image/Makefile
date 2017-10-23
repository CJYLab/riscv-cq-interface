TARGET = mnist-show-image

C_SRCS := mnist_show_image.c
CFLAGS += -O3 -fno-builtin-printf

# LDFLAGS += train-images-idx3-ubyte.100.o
LDFLAGS += t10k-images-idx3-ubyte.o
LDFLAGS += t10k-labels-idx1-ubyte.o
LDFLAGS += wb0_init.o
LDFLAGS += wb1_init.o
LDFLAGS += wh0_init.o
LDFLAGS += wh1_init.o

LINK_DEPS += train_data
LINK_DEPS += t10k_data
# LINK_DEPS += train-images-idx3-ubyte.100.o
LINK_DEPS += t10k-images-idx3-ubyte.o
LINK_DEPS += t10k-labels-idx1-ubyte.o
LINK_DEPS += wb0_init.o
LINK_DEPS += wb1_init.o
LINK_DEPS += wh0_init.o
LINK_DEPS += wh1_init.o

CLEAN_OBJS += train-images-idx3-ubyte.100.o
CLEAN_OBJS += wb0_init.o
CLEAN_OBJS += wb1_init.o
CLEAN_OBJS += wh0_init.o
CLEAN_OBJS += wh1_init.o


$(DHRY_OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DHRY_CFLAGS) -c -o $@ $<

BSP_BASE = ../../bsp
include $(BSP_BASE)/env/common.mk

IMAGE_SIZE = 4000

train-images-idx3-ubyte.100.o: train-images-idx3-ubyte
	head -c $(IMAGE_SIZE) $< > $<.100
	riscv64-unknown-elf-objcopy -I binary -O elf32-littleriscv -B riscv --rename-section .data=.rodata $<.100 $@

t10k-images-idx3-ubyte.o: t10k-images-idx3-ubyte
	riscv64-unknown-elf-objcopy -I binary -O elf32-littleriscv -B riscv --rename-section .data=.rodata $< $@

t10k-labels-idx1-ubyte.o: t10k-labels-idx1-ubyte
	riscv64-unknown-elf-objcopy -I binary -O elf32-littleriscv -B riscv --rename-section .data=.rodata $< $@

train_data: train-images-idx3-ubyte train-labels-idx1-ubyte
t10k_data: t10k-images-idx3-ubyte t10k-labels-idx1-ubyte

%-ubyte: %-ubyte.gz
	gunzip $^

%.gz:
	wget "http://yann.lecun.com/exdb/mnist/$@"

%_init.o: %.bin
	riscv64-unknown-elf-objcopy -I binary -O elf32-littleriscv -B riscv --rename-section .data=.rodata $< $@
