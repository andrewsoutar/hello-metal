#include "bootloader.h"

#include "mem.h"
#include "multiboot.h"
#include "term.h"

static struct multiboot2_info *info;

#define ALIGN(x, width) ((((x) - 1) & ~((width) - 1)) + (width))

static struct multiboot2_info_tag *find_tag(uint32_t type) {
  char *tagp = info->tags;
  while (tagp < info->tags + info->total_size) {
    struct multiboot2_info_tag *tag = (struct multiboot2_info_tag *) tagp;
    tagp += ALIGN(tag->size, 8);

    if (tag->type == MULTIBOOT2_INFO_END)
      break;
    if (tag->type == type)
      return tag;
  }
  return NULL;
}

static int multiboot_early_init(uint32_t info_) {
  if ((info = map_region_early(info_, sizeof *info)) == NULL)
    return -1;
  return 0;
}

static int multiboot_term_init() {
  struct multiboot2_info_framebuffer *framebuffer_data;
  if ((framebuffer_data = (struct multiboot2_info_framebuffer *) find_tag(MULTIBOOT2_INFO_FRAMEBUFFER)) == NULL)
    return -1;
  if (framebuffer_data->type == MULTIBOOT2_FRAMEBUFFER_TEXT) {
    /* FIXME we need to make sure this memory is mapped, but it works for now */
    term_init((void *) (uintptr_t) framebuffer_data->addr,
              framebuffer_data->width, framebuffer_data->height);
    term_print("Hello, world! We have a terminal\n");
    return 0;
  }
  return -1;
}

static int multiboot_physmem_init() {
  return 0;
}

static int multiboot_init() {
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

tags_done:
  return 0;
}

struct bootloader multiboot_bootloader = {
  .early_init = &multiboot_early_init,
  .term_init = &multiboot_term_init,
  .physmem_init = &multiboot_physmem_init,
  .init = &multiboot_init
};
