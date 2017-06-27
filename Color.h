#ifndef LIB_COLOR_H_
#define LIB_COLOR_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <dirent.h>
#include <stdio.h>
#include "String.h"

/* Black 0;30 */
/* Dark Gray 1;30 */
/* Blue 0;34 */
/* Light Blue 1;34 */
/* Green 0;32 */
/* Light Green 1;32 */
/* Cyan 0;36 */
/* Light Cyan 1;36 */
/* Red 0;31 */
/* Light Red 1;31 */
/* Purple 0;35 */
/* Light Purple 1;35 */
/* Brown 0;33 */
/* Yellow 1;33 */
/* Light Gray 0;37 */
/* White 1;37 */

char *va_format(int initial, char *fmt, ...);
int va_system(char *fmt, ...);

const static char *_customizedFormat = "\e[%d;%d;%dm%s\e[0m";

const static char *_coloredFormat = "\e[%d;%dm%s\e[0m";

typedef enum AttrCode {
	none = 0, bold = 1, underscore = 4, blink = 5, reverse = 7, concealed = 8
} AttrCode;

typedef enum TextColorCode {
	black = 30,
	red = 31,
	green = 32,
	yellow = 33,
	blue = 34,
	magenta = 35,
	cyan = 36,
	white = 37
} TextColorCode;

typedef enum BgColorCode {
	BLACK = 40,
	RED = 41,
	GREEN = 42,
	YELLOW = 43,
	BLUE = 44,
	MAGENTA = 45,
	CYAN = 46,
	WHITE = 47
} BgColorCode;

#define COLOR_CODE_LEN  sizeof(_customizedFormat) + 2 * 3 + 1

// construct a string with attribute, textColor and background
// color, return the c-str of it. the str should be inited
static inline String *
customizedString(int attrCode, int textColor, int bgColor, const char *str) {
	int len = strlen(str) + COLOR_CODE_LEN;
	String *s = newString(len);
	String_format(s, _customizedFormat, attrCode, textColor, bgColor, str);
	return s;
}

static inline String*
coloredString(int attrCode, int textColor, const char *s) {
	int len = strlen(s) + COLOR_CODE_LEN;
	String *str = newString(len);
	String_format(str, _coloredFormat, attrCode, textColor, s);
	return str;
}

#define executable_color(e) coloredString(bold, green, e)
#define directory_color(d) coloredString(bold, blue, d)
#define compress_color(c) coloredString(bold , red, c)
#define plainfile_color(f) coloredString(none, white, f)
#define symbol_color(s) coloredString(bold, cyan, s)
#define device_color(d) customizedString(bold, yellow, BLACK, d)

#endif 
