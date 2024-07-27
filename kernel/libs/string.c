#include <stdbool.h>
#include <libs/string.h>

bool str_contains_char(const char* s, const char c) {
    while (*s) {
        if (*s == c)
            return true;
        s++;
    }
    return false;
}

void str_shift_left(char *str, int shift) {
    int i = 0;
    
    // Move the pointer `shift` positions to the left
    while (str[i + shift] != '\0') {
        str[i] = str[i + shift];
        i++;
    }
    
    // Null-terminate the new string
    str[i] = '\0';
}