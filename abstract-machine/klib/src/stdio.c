#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list valist;
	va_start(valist, fmt);
	int i = 0;
	for(; *(fmt + i) != '\0'; i ++)
	{
		if(*(fmt + i) == '%')
		{
			switch(*(fmt + i + 1))
			{
				case 'd':
					*(out + i) = (char)(va_arg(valist, int) + '0');
					i ++;
				case 's':
					char *p = va_arg(valist, char *);
					for(int j = 0; *(p + j) != '\0'; j ++, i ++)
						*(out + i) = *(p + j);
			}
		}
		else *(out + i) = *(fmt + i);
	}
	va_end(valist);
	return i;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
