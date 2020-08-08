#include <stddef.h>

#include "hlt.h"
#include "mem.h"
#include "multiboot.h"
#include "term.h"

#define ALIGN(x, width) ((((x) - 1) & ~((width) - 1)) + (width))

void main(uint32_t magic, uint32_t info_) {
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    return;

  /* Map the multiboot info structure */
  struct multiboot2_info *info = map_region_early(info_, sizeof *info);

  {
    char *tagp = info->tags;
    while (tagp < info->tags + info->total_size) {
      struct multiboot2_info_tag *tag = (struct multiboot2_info_tag *) tagp;
      tagp += ALIGN(tag->size, 8);

      if (tag->type == MULTIBOOT2_INFO_END)
        break;
      if (tag->type != MULTIBOOT2_INFO_FRAMEBUFFER)
        continue;

      struct multiboot2_info_framebuffer *framebuffer_data;
      if (tag->size < sizeof *framebuffer_data)
        continue;

      framebuffer_data = (struct multiboot2_info_framebuffer *) tag;
      if (framebuffer_data->type == MULTIBOOT2_FRAMEBUFFER_TEXT) {
        term_init((void *) (uintptr_t) framebuffer_data->addr,
                  framebuffer_data->width, framebuffer_data->height);
        term_print("Hello, world! We have a terminal\n");
      }
    }
  }

  {
    char *tagp = info->tags;
    while (tagp < info->tags + info->total_size) {
      struct multiboot2_info_tag *tag = (struct multiboot2_info_tag *) tagp;

      switch (tag->type) {
      case MULTIBOOT2_INFO_END:
        goto tags_done;

      case MULTIBOOT2_INFO_CMDLINE: {
        struct multiboot2_info_cmdline *cmdline_data;
        if (tag->size < sizeof *cmdline_data)
          break;
        cmdline_data = (struct multiboot2_info_cmdline *) tag;
        if (tag->size > sizeof *cmdline_data) {
          cmdline_data->string[tag->size - sizeof *cmdline_data - 1] = '\0';
          term_print("Command line: ");
          term_print(cmdline_data->string);
          term_print("\n");
        }
      }
        break;

      case MULTIBOOT2_INFO_MODULE: {
        struct multiboot2_info_module *module_data;
        if (tag->size < sizeof *module_data)
          break;
        module_data = (struct multiboot2_info_module *) tag;
        if (tag->size > sizeof *module_data) {
          module_data->string[tag->size - sizeof *module_data - 1] = '\0';
          term_print("Module ");
          term_print(module_data->string);
          term_print(": ");
        } else {
          term_print("Module <unknown>: ");
        }
        term_print_u32(module_data->start);
        term_print("-");
        term_print_u32(module_data->end);
        term_print("\n");
        term_printn((char *) (uintptr_t) module_data->start, module_data->end - module_data->start);
        term_print("\n");
      }
        break;
      }

      tagp += ALIGN(tag->size, 8);
    }
  tags_done:;
  }

  /* Hello, metal! */
  term_print("Hello, bare metal!\n");

 err:
  for (;;) hlt();
}
