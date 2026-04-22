/* SPDX-License-Identifier: 0BSD */

/* Copyright (C) 2022-2026 Mintsuki and contributors.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LIMINE_H
#define LIMINE_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LIMINE_PTR_WIDTH
#if LIMINE_PTR_WIDTH == 64
#define LIMINE_PTR(TYPE) uint64_t
#elif LIMINE_PTR_WIDTH == 32
#define LIMINE_PTR(TYPE) uint32_t
#else
#error "Invalid LIMINE_PTR_WIDTH"
#endif
#else
#define LIMINE_PTR(TYPE) TYPE
#endif

#define LIMINE_BASE_REVISION(N) {0xf9562b2d5c95a6c8, 0x6a7b384944536bdc, (N)}
#define LIMINE_BASE_REVISION_SUPPORTED(base_revision) ((base_revision)[2] == 0)

#define LIMINE_REQUESTS_START_MARKER                                           \
  {0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,                 \
   0x181e920a7852b9d9}

#define LIMINE_REQUESTS_END_MARKER                                             \
  {0xadc0e0531bb10d03, 0x9572709f31764c62, 0x2b9e5c34c8c880e0,                 \
   0x88f11db09487c22e}

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

struct limine_uuid {
  uint32_t a;
  uint16_t b;
  uint16_t c;
  uint8_t d[8];
};

#define LIMINE_MEDIA_TYPE_GENERIC 0
#define LIMINE_MEDIA_TYPE_OPTICAL 1
#define LIMINE_MEDIA_TYPE_TFTP 2

struct limine_file {
  uint64_t revision;
  LIMINE_PTR(void *) address;
  uint64_t size;
  LIMINE_PTR(char *) path;
  LIMINE_PTR(char *) string;
  uint32_t media_type;
  uint32_t unused;
  uint32_t tftp_ip;
  uint32_t tftp_port;
  uint32_t partition_index;
  uint32_t mbr_disk_id;
  struct limine_uuid gpt_disk_uuid;
  struct limine_uuid gpt_part_uuid;
  struct limine_uuid part_uuid;
};

/* Bootloader info */
#define LIMINE_BOOTLOADER_INFO_REQUEST_ID                                      \
  {LIMINE_COMMON_MAGIC, 0xf55038d8e2a1202f, 0x279426fcf5f59740}

struct limine_bootloader_info_response {
  uint64_t revision;
  LIMINE_PTR(char *) name;
  LIMINE_PTR(char *) version;
};

struct limine_bootloader_info_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_bootloader_info_response *) response;
};

/* Framebuffer */
#define LIMINE_FRAMEBUFFER_REQUEST_ID                                          \
  {LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b}
#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_video_mode {
  uint64_t pitch;
  uint64_t width;
  uint64_t height;
  uint16_t bpp;
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
};

struct limine_framebuffer {
  LIMINE_PTR(void *) address;
  uint64_t width;
  uint64_t height;
  uint64_t pitch;
  uint16_t bpp;
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
  uint8_t unused[7];
  uint64_t edid_size;
  LIMINE_PTR(void *) edid;
  uint64_t mode_count;
  LIMINE_PTR(struct limine_video_mode **) modes;
};

struct limine_framebuffer_response {
  uint64_t revision;
  uint64_t framebuffer_count;
  LIMINE_PTR(struct limine_framebuffer **) framebuffers;
};

struct limine_framebuffer_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_framebuffer_response *) response;
};

/* Memory map */
#define LIMINE_MEMMAP_REQUEST_ID                                               \
  {LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62}

#define LIMINE_MEMMAP_USABLE 0
#define LIMINE_MEMMAP_RESERVED 1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE 2
#define LIMINE_MEMMAP_ACPI_NVS 3
#define LIMINE_MEMMAP_BAD_MEMORY 4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES 6
#define LIMINE_MEMMAP_FRAMEBUFFER 7
#define LIMINE_MEMMAP_RESERVED_MAPPED 8

struct limine_memmap_entry {
  uint64_t base;
  uint64_t length;
  uint64_t type;
};

struct limine_memmap_response {
  uint64_t revision;
  uint64_t entry_count;
  LIMINE_PTR(struct limine_memmap_entry **) entries;
};

struct limine_memmap_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_memmap_response *) response;
};

/* HHDM */
#define LIMINE_HHDM_REQUEST_ID                                                 \
  {LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b}

struct limine_hhdm_response {
  uint64_t revision;
  uint64_t offset;
};

struct limine_hhdm_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_hhdm_response *) response;
};

/* RSDP */
#define LIMINE_RSDP_REQUEST_ID                                                 \
  {LIMINE_COMMON_MAGIC, 0xc5e77b6b397e7b43, 0x27637845accdcf3c}

struct limine_rsdp_response {
  uint64_t revision;
  LIMINE_PTR(void *) address;
};

struct limine_rsdp_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_rsdp_response *) response;
};

/* Entry point */
#define LIMINE_ENTRY_POINT_REQUEST_ID                                          \
  {LIMINE_COMMON_MAGIC, 0x13d86c035a1cd3e1, 0x2b0caa89d8f3026a}
typedef void (*limine_entry_point)(void);

struct limine_entry_point_response {
  uint64_t revision;
};

struct limine_entry_point_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_entry_point_response *) response;
  LIMINE_PTR(limine_entry_point) entry;
};

/* Firmware type */
#define LIMINE_FIRMWARE_TYPE_REQUEST_ID                                        \
  {LIMINE_COMMON_MAGIC, 0x8c2f75d90bef28a8, 0x7045a4688eac00c3}

#define LIMINE_FIRMWARE_TYPE_X86_BIOS 0
#define LIMINE_FIRMWARE_TYPE_UEFI32 1
#define LIMINE_FIRMWARE_TYPE_UEFI64 2

struct limine_firmware_type_response {
  uint64_t revision;
  uint64_t firmware_type;
};

struct limine_firmware_type_request {
  uint64_t id[4];
  uint64_t revision;
  LIMINE_PTR(struct limine_firmware_type_response *) response;
};

#ifdef __cplusplus
}
#endif

#endif /* LIMINE_H */
