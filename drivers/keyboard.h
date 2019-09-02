/* File: keyboard.h
 *
 */

#include <stdint.h>
#include "../pit.h"
#include "../vga_text_mode.h"
#include "../lib/string.h"
/* remove "../" and use $INCLUDE path */

#ifndef KBD_H
#define KBD_H

/* PS/2 keyboard commands */
#define KBD_SET_LEDS         0xed
#define KBD_DEVICE_ECHO      0xee
#define KBD_SCAN_CODE_SET    0xf0
#define KBD_IDENTIFY         0xf2
#define KBD_AUTO_REPEAT_RATE 0xf3
#define KBD_ENABLE_SCANNING  0xf4
#define KBD_DISABLE_SCANNING 0xf5
#define KBD_SET_DEFAULT      0xf6

#define KBD_ALL_KEYS_AUTO_REPEAT_ONLY         0xf7 /* scancode set 3 only */
#define KBD_ALL_KEYS_MAKE_RELEASE             0xf8 /* scancode set 3 only */
#define KBD_ALL_KEYS_MAKE_ONLY                0xf9 /* scancode set 3 only */
#define KBD_ALL_KEYS_AUTO_REPEAT_MAKE_RELEASE 0xfa /* scancode set 3 only */
#define KBD_SINGLE_KEY_AUTO_REPEAT_ONLY       0xfb /* scancode set 3 only */
#define KBD_SINGLE_KEY_MAKE_RELEASE           0xfc /* scancode set 3 only */
#define KBD_SINGLE_KEY_MAKE_ONLY              0xfd /* scancode set 3 only */

#define KBD_RESEND 0xfe
#define KBD_RESET  0xff

/* Response bytes */
#define KEY_DETECTION_OR_BUFFER_ERROR1 0x00
#define SELF_TEST_PASSED               0xaa
#define ECHO_RESPONSE                  0xee
#define COMMAND_ACK                    0xfa
#define SELF_TEST_FAILED1              0xfc
#define SELF_TEST_FAILED2              0xfd
#define RESEND_COMMAND                 0xfe
#define KEY_DETECTION_OR_BUFFER_ERROR2 0xff

/* Scan code Set 2 */
#define SCANCODE_A 0x1c
#define SCANCODE_B 0x32
#define SCANCODE_C 0x21
#define SCANCODE_D 0x23
#define SCANCODE_E 0x24
#define SCANCODE_F 0x2b
#define SCANCODE_G 0x34
#define SCANCODE_H 0x33
#define SCANCODE_I 0x43
#define SCANCODE_J 0x3b
#define SCANCODE_K 0x42
#define SCANCODE_L 0x4b
#define SCANCODE_M 0x3a
#define SCANCODE_N 0x31
#define SCANCODE_O 0x44
#define SCANCODE_P 0x4d
#define SCANCODE_Q 0x15
#define SCANCODE_R 0x2d
#define SCANCODE_S 0x1b
#define SCANCODE_T 0x2c
#define SCANCODE_U 0x3c
#define SCANCODE_V 0x2a
#define SCANCODE_W 0x1d
#define SCANCODE_X 0x22
#define SCANCODE_Y 0x35
#define SCANCODE_Z 0x1a

#define SCANCODE_0 0x45
#define SCANCODE_1 0x16
#define SCANCODE_2 0x1e
#define SCANCODE_3 0x26
#define SCANCODE_4 0x25
#define SCANCODE_5 0x2e
#define SCANCODE_6 0x36
#define SCANCODE_7 0x3d
#define SCANCODE_8 0x3e
#define SCANCODE_9 0x46

#define SCANCODE_BKTICK  0x0e
#define SCANCODE_HYPHEN  0x4e
#define SCANCODE_EQUAL   0x55
#define SCANCODE_BKSLASH 0x5d
#define SCANCODE_BKSPC   0x66
#define SCANCODE_SPACE   0x29
#define SCANCODE_TAB     0x0d
#define SCANCODE_CAPS    0x58

#define SCANCODE_LSHFT 0x12
#define SCANCODE_LCTRL 0x14
#define SCANCODE_LGUI  0xe01f	/* break: e0, f0, 1f */
#define SCANCODE_LALT  0x11
#define SCANCODE_RSHFT 0x59
#define SCANCODE_RCTRL 0xe014	/* break: e0, f0, 14 */
#define SCANCODE_RGUI  0xe027	/* break: e0, f0, 27 */
#define SCANCODE_RALT  0xe011	/* break: e0, f0, 11 */

#define SCANCODE_APPS 0xe02f	/* break: e0, f0, 2f */
#define SCANCODE_RETN 0x5a
#define SCANCODE_ESC  0x76

#define SCANCODE_F1  0x05
#define SCANCODE_F2  0x06
#define SCANCODE_F3  0x04
#define SCANCODE_F4  0x0c
#define SCANCODE_F5  0x03
#define SCANCODE_F6  0x0b
#define SCANCODE_F7  0x83
#define SCANCODE_F8  0x0a
#define SCANCODE_F9  0x01
#define SCANCODE_F10 0x09
#define SCANCODE_F11 0x78
#define SCANCODE_F12 0x07

#define SCANCODE_PRNTSCRN 0xe012e07c /* break: e0, f0, 7c, e0, f0, 12 */
#define SCANCODE_SCROLL   0x7e
#define SCANCODE_PAUSE    0xe11477e1f014f077 /* make: e1, 14, 77, e1, f0, 14, f0, 77 */
#define SCANCODE_LBRACKET 0x54

#define SCANCODE_INSERT 0xe070	/* break: e0, f0, 70 */
#define SCANCODE_HOME   0xe06c	/* break: e0, f0, 6c */
#define SCANCODE_PGUP   0xe07d	/* break: e0, f0, 7d */
#define SCANCODE_DELETE 0xe071	/* break: e0, f0, 71 */
#define SCANCODE_END    0xe069	/* break: e0, f0, 69 */
#define SCANCODE_PGDN   0xe07a	/* break: e0, f0, 7a */

#define SCANCODE_UARROW 0xe075	/* break: e0, f0, 75 */
#define SCANCODE_LARROW 0xe06b 	/* break: e0, f0, 6b */
#define SCANCODE_DARROW 0xe072	/* break: e0, f0, 72 */
#define SCANCODE_RARROW 0xe074	/* break: e0, f0, 74 */

#define SCANCODE_NUM 0x77

#define SCANCODE_KP_FSLASH   0xe04a /* break: e0, f0, 4a */
#define SCANCODE_KP_ASTERISK 0x7c
#define SCANCODE_KP_MINUS    0x7b
#define SCANCODE_KP_PLUS     0x79
#define SCANCODE_KP_EN       0xe05a	/* break: e0, f0, 5a */
#define SCANCODE_KP_PERIOD   0x71

#define SCANCODE_KP_0 0x70
#define SCANCODE_KP_1 0x69
#define SCANCODE_KP_2 0x72
#define SCANCODE_KP_3 0x7a
#define SCANCODE_KP_4 0x6b
#define SCANCODE_KP_5 0x73
#define SCANCODE_KP_6 0x74
#define SCANCODE_KP_7 0x6c
#define SCANCODE_KP_8 0x75
#define SCANCODE_KP_9 0x7d

#define SCANCODE_RBRACKET 0x5b
#define SCANCODE_SEMICOLN 0x4c
#define SCANCODE_SNGLQUOT 0x52
#define SCANCODE_COMMA    0x41
#define SCANCODE_PERIOD   0x49
#define SCANCODE_FSLASH   0x4a

/* ACPI Scan Codes */
#define SCANCODE_POWER 0xe037	/* break: e0, f0, 37 */
#define SCANCODE_SLEEP 0xe03f	/* break: e0, f0, 3f */
#define SCANCODE_WAKE  0xe05e	/* break: e0, f0, 5e */

/* Windows Multimedia Scan Codes */
#define SCANCODE_NEXT_TRACK 0xe04d
#define SCANCODE_PREV_TRACK 0xe015
#define SCANCODE_STOP       0xe03b
#define SCANCODE_PLAY_PAUSE 0xe034
#define SCANCODE_MUTE       0xe023
#define SCANCODE_VOL_UP     0xe032
#define SCANCODE_VOL_DN     0xe021

#define SCANCODE_MEDIA_SEL 0xe050
#define SCANCODE_EMAIL     0xe048
#define SCANCODE_CALC      0xe02b
#define SCANCODE_MY_COMP   0xe040

#define SCANCODE_WWW_SEARCH    0xe010
#define SCANCODE_WWW_HOME      0xe03a
#define SCANCODE_WWW_BACK      0xe038
#define SCANCODE_WWW_FWD       0xe030
#define SCANCODE_WWW_STOP      0xe028
#define SCANCODE_WWW_REFRESH   0xe020
#define SCANCODE_WWW_FAVORITES 0xe018


#define SCANCODE_BUFFER_MAX_SIZE 32

extern char scancode_buffer[SCANCODE_BUFFER_MAX_SIZE];
extern int buffer_size;

void keyboard_interrupt ();


#endif	/* KBD_H */
