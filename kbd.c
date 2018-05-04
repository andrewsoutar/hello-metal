#include "kbd.h"

#include <stddef.h>
#include <stdint.h>

#include "cpuio.h"
#include "hlt.h"
#include "interrupt.h"
#include "string.h"
#include "term.h"

#define PS2_STATUS_OUTPUT_FULL (1u<<0)
#define PS2_STATUS_INPUT_FULL  (1u<<1)
static uint8_t ps2_status(void) {
  return inb(0x64);
}

#define PS2_READ_CONFIG    (0x20)
#define PS2_WRITE_CONFIG   (0x60)
#define PS2_DISABLE_SECOND (0xA7)
#define PS2_ENABLE_SECOND  (0xA8)
#define PS2_PORT_TEST2     (0xA9)
#define PS2_SELF_TEST      (0xAA)
#define PS2_PORT_TEST1     (0xAB)
#define PS2_DISABLE_FIRST  (0xAD)
#define PS2_ENABLE_FIRST   (0xAE)
static void ps2_cmd(uint8_t cmd) {
  while (ps2_status() & PS2_STATUS_INPUT_FULL);
  outb(0x64, cmd);
}

static void ps2_write(uint8_t data) {
  while (ps2_status() & PS2_STATUS_INPUT_FULL);
  outb(0x60, data);
}

static uint8_t ps2_read(void) {
  while (!(ps2_status() & PS2_STATUS_OUTPUT_FULL));
  return inb(0x60);
}

/* PS/2 Configuration Byte */
#define PS2_CONFIG_INT1  (1u<<0)
#define PS2_CONFIG_INT2  (1u<<1)
#define PS2_CONFIG_SYS   (1u<<2)
#define PS2_CONFIG_CLK1  (1u<<4)
#define PS2_CONFIG_CLK2  (1u<<5)
#define PS2_CONFIG_TRANS (1u<<6)

static size_t ps2_kbd_queue_top = 0;
static size_t ps2_kbd_queue_bot = 0;
static uint16_t ps2_kbd_queue[8] = {0}; /* Ought to be enough */
static void ps2_kbd_enqueue(uint16_t cmd) {
  if (ps2_kbd_queue_top == ps2_kbd_queue_bot) {
    if (cmd & 0xFF00)
      ps2_write(cmd >> 8);
    ps2_write(cmd & 0xFF);
    return;
  }
  while (ps2_kbd_queue_top >= ps2_kbd_queue_bot + 8)
    hlt();
  ps2_kbd_queue[ps2_kbd_queue_top++ % 8] = cmd;
}
static uint16_t ps2_kbd_repeat(void) {
  return ps2_kbd_queue[ps2_kbd_queue_bot];
}
static uint16_t ps2_kbd_next(void) {
  if (++ps2_kbd_queue_bot < ps2_kbd_queue_top)
    return ps2_kbd_queue[ps2_kbd_queue_bot];
  else
    return 0;
}

static const char scancode_map[] =
  "\xff\xff""1234567890-=\b\t"
  "qwertyuiop[]\n\0as"
  "dfghjkl;'`\0\\zxcv"
  "bnm,./\0\0\0 \0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\x80\0\0\x81\0\x82\0\0\x83";

static void handle_kbd_int(uint32_t interrupt, uint32_t ignored) {
  (void) ignored;
  uint8_t data = ps2_read();
  if (interrupt != 0x21)
    return;
  if (ps2_kbd_queue_top != ps2_kbd_queue_bot) {
    if (data == 0xFA) {
      uint16_t next = ps2_kbd_next();
      if (next) {
        if (next & 0xFF00)
          ps2_write(next >> 8);
        ps2_write(next & 0xFF);
      }
    } else if (data == 0xFE) {
      uint16_t next = ps2_kbd_repeat();
      if (next & 0xFF00)
        ps2_write(next >> 8);
      ps2_write(next & 0xFF);
    }
  } else {
    if (data < sizeof(scancode_map) && scancode_map[data] && scancode_map[data] != '\xff') {
      static char buf[255] = {0};
      static size_t buf_gap_start = 0, buf_gap_end = 8, buf_len = 8;
      char chr = scancode_map[data];
      switch (chr) {
      case '\n':
        for (size_t i = buf_len - buf_gap_end; i > 0; --i)
          buf[buf_gap_start + i-1] = buf[buf_gap_end + i-1];
        buf[buf_len - (buf_gap_end - buf_gap_start)] = '\0';
        buf_gap_start = 0, buf_gap_end = 8, buf_len = 8;
        term_print("\nSegmentation fault (core dumped)");
        term_print("\nnqnsminishell> ");
        break;
      case '\b':
        if (buf_gap_start) {
          --buf_gap_start;
          term_put_char('\b');
          for (size_t i = buf_gap_end; i < buf_len; ++i)
            term_put_char(buf[i]);
          term_put_char(' ');
          term_move_x(buf_gap_end - buf_len - 1);
        }
        break;
      case '\x81':
        if (buf_gap_start) {
          buf[--buf_gap_end] = buf[--buf_gap_start];
          term_move_x(-1);
        }
        break;
      case '\x82':
        if (buf_gap_end < buf_len) {
          buf[buf_gap_start++] = buf[buf_gap_end++];
          term_move_x(1);
        }
        break;
      default:
        if (chr < 0)
          break;
        if (buf_gap_start == buf_gap_end) {
          if ((buf_len += 8) > sizeof(buf))
            buf_len = sizeof(buf);
          if ((buf_gap_end += 8) > sizeof(buf))
            buf_gap_end = sizeof(buf);
          for (size_t i = buf_len - buf_gap_end; i > 0; --i)
            buf[buf_gap_end + i-1] = buf[buf_gap_start + i-1];
        }
        if (buf_gap_start < buf_gap_end) {
          buf[buf_gap_start++] = chr;
          term_put_char(chr);
          for (size_t i = buf_gap_end; i < buf_len; ++i)
            term_put_char(buf[i]);
          term_move_x(buf_gap_end - buf_len);
        }
      }
    }
  }
}

void kbd_init(void) {
  uint8_t config;
  int port_1, port_2;
  ps2_cmd(PS2_DISABLE_FIRST);
  ps2_cmd(PS2_DISABLE_SECOND);

  while (ps2_status() & PS2_STATUS_OUTPUT_FULL)
    inb(0x60);

  ps2_cmd(PS2_READ_CONFIG);
  config = ps2_read();
  config &= ~(PS2_CONFIG_INT1 | PS2_CONFIG_INT2 | PS2_CONFIG_TRANS);
  ps2_cmd(PS2_WRITE_CONFIG);
  ps2_write(config);

  ps2_cmd(PS2_SELF_TEST);
  if (ps2_read() != 0x55) {
    term_print("PS/2 device failed self-test\n");
    return;
  }

  if (!(config & PS2_CONFIG_CLK2)) {
    port_2 = 0;
  } else {
    /* Try to detect */
    ps2_cmd(PS2_ENABLE_SECOND);
    ps2_cmd(PS2_READ_CONFIG);
    if (ps2_read() & PS2_CONFIG_CLK2) {
      port_2 = 0;
    } else {
      port_2 = 1;
      ps2_cmd(PS2_DISABLE_SECOND);
    }
  }

  ps2_cmd(PS2_PORT_TEST1);
  if (ps2_read() != 0x00) {
    term_print("PS/2 Port 1 failed self-test\n");
    port_1 = 0;
  } else {
    port_1 = 1;
  }

  if (port_2) {
    ps2_cmd(PS2_PORT_TEST2);
    if (ps2_read() != 0x00) {
      term_print("PS/2 Port 2 failed self-test\n");
      port_2 = 0;
    }
  }

  if (!(port_1 || port_2)) {
    term_print("No ports left, giving up.\n");
    return;
  }

  if (port_1)
    config |= PS2_CONFIG_INT1;
  if (port_2)
    config |= PS2_CONFIG_INT2;
  ps2_cmd(PS2_WRITE_CONFIG);
  ps2_write(config);

  add_handler(0x21, &handle_kbd_int);
  add_handler(0x2C, &handle_kbd_int);

  if (port_1)
    ps2_cmd(PS2_ENABLE_FIRST);
  if (port_2)
    ps2_cmd(PS2_ENABLE_SECOND);

  ps2_kbd_enqueue(0xF001);
  ps2_kbd_enqueue(0xF4);
}
