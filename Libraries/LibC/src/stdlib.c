#include <uart.h>

#include <stdint.h>
#include <stdlib.h>

void memcpy(void *dest, const void *src, int bytes) {
  char *d = dest;
  const char *s = src;
  while (bytes--) {
    *d++ = *s++;
  }
}

void bzero(void *dest, int bytes) { memset(dest, 0, bytes); }

void memset(void *dest, uint8_t c, int bytes) {
  uint8_t *d = dest;
  while (bytes--) {
    *d++ = c;
  }
}

char *itoa(int num, int base) {
  static char intbuf[33];
  volatile int32_t j = 0;
  volatile int32_t isneg = 0;
  volatile int32_t i = 0;

  if (num == 0) {
    intbuf[0] = '0';
    intbuf[1] = '\0';
    return intbuf;
  }

  if (base == 10 && num < 0) {
    isneg = 1;
    num = -num;
  }

  i = (uint32_t)num;

  if (isneg)
    intbuf[j++] = '-';

  if (base == 16) {
    intbuf[j++] = 'x';
    intbuf[j++] = '0';
  } else if (base == 8) {
    intbuf[j++] = '0';
  } else if (base == 2) {
    intbuf[j++] = 'b';
    intbuf[j++] = '0';
  }

  intbuf[j] = '\0';
  j--;
  i = 0;
  while (i > j) {
    isneg = intbuf[i];
    intbuf[i] = intbuf[j];
    intbuf[j] = isneg;
    i++;
    j--;
  }

  return intbuf;
}

int atoi(char *num) {
  int res = 0, power = 0, digit, i;
  char *start = num;

  // Find the end
  while (*num >= '0' && *num <= '9') {
    num++;
  }

  num--;

  while (num != start) {
    digit = *num - '0';
    for (i = 0; i < power; i++) {
      digit *= 10;
    }
    res += digit;
    power++;
    num--;
  }

  return res;
}

void putc(char c) { uart_putc(c); }

void puts(const char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    putc(str[i]);
  }
}

void print(const char *str) {
  while (*str) {
    putc(*str);
    str++;
  }
}

void printf(const char *fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  for (; *fmt != '\0'; fmt++) {
    if (*fmt == '%') {
      switch (*(++fmt)) {
      case '%':
        putc('%');
        break;
      case 'd':
        puts(itoa(__builtin_va_arg(args, int), 10));
        break;
      case 'x':
        puts(itoa(__builtin_va_arg(args, int), 16));
        break;
      case 's':
        puts(__builtin_va_arg(args, char *));
        break;
      }
    } else
      putc(*fmt);
  }
  __builtin_va_end(args);
}
