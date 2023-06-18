#ifndef TRIVIA_NIGHT_OS_DEPENDENT_H
#define TRIVIA_NIGHT_OS_DEPENDENT_H

#ifdef __linux__
#define CLEAR "clear"

#define VK_DOWN 66
#define VK_UP 65
#define VK_LEFT 68
#define VK_RIGHT 67
#else
#include <conio.h> // ::getch()

#define CLEAR "cls"

#define VK_DOWN 80
#define VK_UP 72
#define VK_LEFT 75
#define VK_RIGHT 77
#endif //__linux__

namespace OSDEP {
    // This function should return the keystroke without allowing it to echo on screen
    inline int getch() {
        int c;

#ifdef __linux__
        system("stty raw");    // Raw input - wait for only a single keystroke
        system("stty -echo");  // Echo off
        c = getchar();
        system("stty cooked"); // Cooked input - reset
        system("stty echo");   // Echo on - Reset
#else
        c = ::getch();
#endif
        return c;
    }
}

#endif //TRIVIA_NIGHT_OS_DEPENDENT_H
