#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/*
void itoa_ten(int number, char *str)
{
	int i = 0;
	char temp[100];
	while(number)
	{
		*(temp + i) = (char)(number % 10 + '0');
		number /= 10;
		i ++;
	}
	for(int j = i - 1, k = 0; j >= 0 ; j --, k ++)
	{
		*(str + k) = *(temp + j);
	}
	*(str + i) = '\0';
}
*/

/*
int printf(const char *fmt, ...) {
	va_list valist;
	va_start(valist, fmt);
	int i = 0, sum = 0;
	char out[100];
	for(int i = 0; *(out + i); i ++)
		*(out + i) = '\0';
	for(; *(fmt + i) != '\0'; i ++)
	{
		if(*(fmt + i) == '%')
		{
			switch(*(fmt + i + 1))
			{
				case 'd':
					char number[100];
					itoa_ten(va_arg(valist, int), number);
					strcat(out, number);
					while(*(out + sum)) sum ++;
					break;
				case 's':
					char *p = va_arg(valist, char *);
					strcat(out, p);
					while(*(out + sum)) sum ++;
					break;
			}
			i ++;
		}
		else
		{
			*(out + sum) = *(fmt + i);
			sum ++;
		}
	}
	*(out + sum + 1) = '\0';
	va_end(valist);
	putstr(out);
	return sum;
	//panic("Not implemented");
}
int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}
*/

/*
int sprintf(char *out, const char *fmt, ...) {
	va_list valist;
	va_start(valist, fmt);
	int i = 0, sum = 0;
	for(int i = 0; *(out + i); i ++)
		*(out + i) = '\0';
	for(; *(fmt + i) != '\0'; i ++)
	{
		if(*(fmt + i) == '%')
		{
			switch(*(fmt + i + 1))
			{
				case 'd':
					char number[100];
					itoa_ten(va_arg(valist, int), number);
					strcat(out, number);
					while(*(out + sum)) sum ++;
					break;
				case 's':
					char *p = va_arg(valist, char *);
					strcat(out, p);
					while(*(out + sum)) sum ++;
					break;
			}
			i ++;
		}
		else
		{
			*(out + sum) = *(fmt + i);
			sum ++;
		}
	}
	*(out + sum + 1) = '\0';
	va_end(valist);
	return sum;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}
*/

static char* get_int(char *p, va_list *ap) {
	int d = va_arg(*ap, int);
	char str[32];
	int len = 0;
	if (d == 0) {
		*p++ = '0';
		return p;
	}
	if (d < 0) {
		*p++ = '-';
		d *= -1;
	}
	while (d) {
		str[len++] = d % 10 + '0';
		d /= 10;
	}
	for (int i = len-1; i >= 0; i--) {
		*p++ = str[i];
	}
	return p;
}

static char* get_string(char* p, va_list *ap) {
	char *str = va_arg(*ap, char*);
	while(*str) {
		*p++ = *str++;
	}
	return p;
}

static char* get_char(char* p, va_list *ap) {
	char ch = (char)va_arg(*ap, int);
	*p++ = ch;
	return p;
}

static int make_out(char *out, const char *fmt, va_list ap) {
	char* p = (char*)out;
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 'd': p = get_int(p, &ap);break;
				case 's': p = get_string(p, &ap);break;
				case 'c': p = get_char(p, &ap);break;
			}
			fmt++;
		}
		else {
			*p++ = *fmt++;
		}
	}
	*p++ = '\0';
	return 0;
}

int printf(const char *fmt, ...) {
  char out[2048];
	va_list ap;
	va_start(ap, fmt);
	make_out(out, fmt, ap);
	va_end(ap);
	putstr(out);
	return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	make_out(out, fmt, ap);
	va_end(ap);
	return 0;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
