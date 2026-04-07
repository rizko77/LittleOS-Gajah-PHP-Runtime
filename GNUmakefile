# ============================================================
# LittleOS Gajah PHP — GNUmakefile
# Kernel murni PHP 8 Runtime · HAL dalam C++
# ============================================================

.SUFFIXES:

override OUTPUT := littleos

# ============================================================
# TOOLCHAIN — C++ Cross-Compiler
# ============================================================

TOOLCHAIN :=
TOOLCHAIN_PREFIX :=

ifneq ($(TOOLCHAIN),)
  ifeq ($(TOOLCHAIN_PREFIX),)
    TOOLCHAIN_PREFIX := $(TOOLCHAIN)-
  endif
endif

# C++ Compiler (BUKAN C!)
ifneq ($(TOOLCHAIN_PREFIX),)
  CXX := $(TOOLCHAIN_PREFIX)g++
else
  CXX := g++
endif

LD := $(TOOLCHAIN_PREFIX)ld

# Cek Clang
override CXX_IS_CLANG := $(shell ! $(CXX) --version 2>/dev/null | grep -q '^Target: '; echo $$?)

ifeq ($(CXX_IS_CLANG),1)
  override CXX += -target x86_64-unknown-none-elf
endif

# ============================================================
# FLAG KOMPILASI C++
# ============================================================

override CXXFLAGS += \
    -Wall \
    -Wextra \
    -std=c++17 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -fno-exceptions \
    -fno-rtti \
    -fno-threadsafe-statics \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mabi=sysv \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel \
    -mgeneral-regs-only \
    -g -O2 -pipe

override CPPFLAGS := \
    -I kernel/include \
    $(CPPFLAGS) \
    -MMD \
    -MP

override LDFLAGS += \
    -m elf_x86_64 \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    --gc-sections \
    -T kernel/linker.lds

# ============================================================
# PENCARIAN FILE SUMBER (.cpp)
# ============================================================

override SRCFILES := $(shell find -L kernel/source -type f -name '*.cpp' 2>/dev/null | LC_ALL=C sort)
override CXXFILES := $(filter %.cpp,$(SRCFILES))
override OBJ := $(addprefix obj/,$(CXXFILES:.cpp=.cpp.o))
override HEADER_DEPS := $(addprefix obj/,$(CXXFILES:.cpp=.cpp.d))

# ============================================================
# TARGET BUILD
# ============================================================

.PHONY: all
all: bin/$(OUTPUT)

-include $(HEADER_DEPS)

# Linking — gabungkan semua .o menjadi kernel ELF
bin/$(OUTPUT): GNUmakefile kernel/linker.lds $(OBJ)
	mkdir -p "$(dir $@)"
	$(LD) $(LDFLAGS) $(OBJ) -o $@
	@echo ""
	@echo "=========================================="
	@echo "  LittleOS Gajah PHP berhasil dibangun!"
	@echo "  Output: bin/$(OUTPUT)"
	@echo "  Kernel: PHP 8 Runtime"
	@echo "=========================================="

# Kompilasi file .cpp
obj/%.cpp.o: %.cpp GNUmakefile
	mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# ============================================================
# CLEAN
# ============================================================

.PHONY: clean
clean:
	rm -rf bin obj

# ============================================================
# ISO — Membuat bootable ISO
# ============================================================

.PHONY: iso
iso: all
	@echo "Membuat ISO bootable..."
	@if [ ! -d limine ]; then \
		echo "Mengunduh Limine bootloader..."; \
		git clone https://codeberg.org/Limine/Limine.git limine --branch=v8.x-binary --depth=1; \
	fi
	mkdir -p iso_root/boot/limine
	mkdir -p iso_root/EFI/BOOT
	cp -v bin/$(OUTPUT) iso_root/boot/
	cp -v boot/limine.conf iso_root/boot/limine/
	cp -v limine/limine-bios.sys iso_root/boot/limine/
	cp -v limine/limine-bios-cd.bin iso_root/boot/limine/
	cp -v limine/limine-uefi-cd.bin iso_root/boot/limine/
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J \
		-b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-hfsplus -apm-block-size 2048 \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o LittleOS.iso
	./limine/limine bios-install LittleOS.iso
	@echo ""
	@echo "=========================================="
	@echo "  ISO LittleOS.iso siap!"
	@echo "  Boot di VirtualBox atau bakar ke USB."
	@echo "=========================================="

.PHONY: run-qemu
run-qemu: iso
	qemu-system-x86_64 -cdrom LittleOS.iso -m 256M -serial stdio
