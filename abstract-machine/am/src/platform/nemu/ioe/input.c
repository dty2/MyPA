#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000
#define CONFIG_I8042_DATA_MMIO 0xa0000060

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t state = *(uint32_t *)(CONFIG_I8042_DATA_MMIO);
  kbd->keycode = state & ~KEYDOWN_MASK;
  if ((state & KEYDOWN_MASK) != 0) {
    kbd->keydown = true;
  }
}
