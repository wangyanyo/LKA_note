#ifndef __KEYBOARD_CLASSIC_H
#define __KEYBOARD_CLASSIC_H

#include "keyboard.h"

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

struct keyboard *classic_init();

#endif