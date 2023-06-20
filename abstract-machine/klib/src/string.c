#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int len = 0;
  assert(s);
  while(*s++ != '\0') len++;
  return len;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  char *p = NULL;
  if(dst == NULL || src == NULL) {
      return NULL;
  }
  p = dst;
  while((*dst++ = *src++) != '\0');
  return p;
	/*
	int i = 0;
	for(; *(src + i) != '\0'; i ++)
		*(dst + i) = *(src + i);
	*(dst + i) = '\0';
	return dst;
	*/
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
	char *p = dst;
	assert((dst != NULL) && (src != NULL));

	while (*dst != '\0') {
		dst++;
	}
	while ((*dst++ = *src++) != '\0') { }
	return p;
	/*
	int i = 0, j = 0;
	while(*(dst + j) != '\0') j ++;
	for(; *(src + i) != '\0'; i ++, j ++)
		*(dst + j) = *(src + i);
	*(dst + j) = '\0';
	return dst;
	*/
  //panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  while ((*s1) && (*s1 == *s2)) {
		s1++;
		s2++;
	}

	if (*(unsigned char*)s1 > *(unsigned char*)s2) {
		return 1;
	}
	else if (*(unsigned char*)s1 < *(unsigned char*)s2) {
		return -1;
	}
	else {
		return 0;
	}
	/*
	int i = 0;
	while(*(s1 + i) && *(s1 + i) == *(s2 + i)) i ++;
	if(*(s1 + i) > *(s2 + i)) return 1;
	else if(*(s1 + i) < *(s2 + i)) return -1;
	else return 0;
	*/
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if (s == NULL || n < 0) {
		return NULL;
	}
	char *pdest = (char *)s;
	while (n-- > 0) {
		*pdest++ = c;
	}
	return s;
	/*
	for(int i = 0; i < n; i ++)
		*((char *)s + i) = c;
	return s;
	*/
  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  if ((in == NULL) || (out == NULL)) return 0;
  while (n--) {
    *(char*)out++ = *(char*)in++;
  }
  return out;
  //panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 != NULL && s2 != NULL && n > 0);
  const char *pdest = (char*)s1;
  const char *psrc = (char*)s2;
  while(*pdest == *psrc && --n > 0) {
    pdest++;
    psrc++;
  }
  int a = *pdest - *psrc;
  if(a > 0) { return 1; }
  else if(a < 0) { return -1; }
	/*
	int i = 0;
	while(*((char *)s1 + i) == *((char *)s2 + i) && i < n) i ++;
	if((*((char *)s1 + i)) > (*((char *)s2 + i)) && i != n) return 1;
	else if(*((char *)s1 + i) < (*((char *)s2 + i)) && i != n) return -1;
	else return 0;
	*/
  //panic("Not implemented");
  return 0;
}

#endif
