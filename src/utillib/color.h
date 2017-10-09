#ifndef COLOR_H_
#define COLOR_H_

#define GREEN "32"
#define RED "31"
#define PURPLE "35"
#define CYAN "36"
#define WHITE "37"

#define COLOR_FORMAT ("%s \e[1;%sm%s\e[0m")
#define MK_COLOR_STRING(c, s) ("\e[1;" c "m" s "\e[0m")
#define GREEN_CARET MK_COLOR_STRING(GREEN, "^")
#define RED_ERROR MK_COLOR_STRING(RED, "error:")
#define RED_FATAL MK_COLOR_STRING(RED, "fatal:")
#define RED_ICE MK_COLOR_STRING(RED, "internal compiler error:")
#define PURPLE_WARNING MK_COLOR_STRING(PURPLE, "warning:")
#define CYAN_NOTE MK_COLOR_STRING(CYAN, "note:")

#endif
