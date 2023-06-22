#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
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
/*
char* itoa(int num, char* buf)
{
  int i = 0;
  int len = 0;
  int flag = 0;
  char tmp;
  if (num == 0) {
    buf[i++] = '0';
  }
  if (num < 0) {
    num *= -1;
    flag = 1;
  }
  while (num) {
    buf[i++] = num % 10 + '0';
    num = num /10;
  }
  if (flag) {
    buf[i++] = '-';
  }
  buf[i] = '\0';
  len = i;
  for (i = 0; i < len / 2; i++) {
    tmp = buf[i];
    buf[i] = buf[len-i-1];
    buf[len-i-1] = tmp;
  }
  return buf;
}

char* xtoa(unsigned int num, char* buf)
{
  int i = 0;
  if (num == 0) {
    buf[i++] = '0';
  }
  while (num) {
    if (num % 16 < 10) {
      buf[i++] = num % 16 + '0';
    }
    else {
      buf[i++] = num % 16 - 10 + 'A';
    }
    num = num / 16;
  }
  buf[i++] = 'x';
  buf[i++] = '0';
  buf[i] = '\0';
  int len = i;

  char tmp;
  for (i = 0; i < len / 2; i++) {
    tmp = buf[i];
    buf[i] = buf[len-i-1];
    buf[len-i-1] = tmp;
  }
  return buf;
}

///////////////////////////////////////////////////////////////////

int printf(const char *fmt, ...) {
  char buf[2048];
  va_list vl_src;
  va_start(vl_src, fmt);
  // 可变参数的转发
  int count = vsprintf(buf, fmt, vl_src);
  putstr(buf);
  va_end(vl_src);
  return count;
}

int vsprintf(char *out, const char *fmt, va_list vl) {
  int count = 0;
  int i = 0;
  while (*fmt != '\0') {
    if (*fmt != '%') {
      out[i++] = *fmt++;
      count++;
      //puts("hit");
    }
    else {
      ++fmt;
      if (*fmt == 's') {
        char* src = va_arg(vl, char*);
        while (*src != '\0') {
          out[i++] = *src++;
          count++;
        }
        ++fmt;
        //puts("shit hit s");
      }
      else if (*fmt == 'd') {
        char buf[33];
        itoa(va_arg(vl, int), buf);
        int index = 0;
        while (buf[index] != '\0') {
          out[i++] = buf[index++];
          count++;
        }
        ++fmt;
        //puts("shit hit d");
      }
      else if (*fmt == 'p') {
        char buf[33];
        xtoa(va_arg(vl, int), buf);
        int index = 0;
        while (buf[index] != '\0') {
          out[i++] = buf[index++];
          count++;
        }
        ++fmt;
      }
      else {
        out[i++] = '%';
        count += 1;
      }
    }
  }
  out[i] = '\0';
  if (count == 0) return -1;
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  int count = vsprintf(out, fmt, vl);
  va_end(vl);
  if (count == 0) return -1;
  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}
*/ 
#endif
