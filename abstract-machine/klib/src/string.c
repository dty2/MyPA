#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
	int i = 0;
	for(; *(dst + i); i ++)
		*(dst + i) = *(src + i);
	return dst;
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
	int i = 0, j = 0;
	while(*(dst + j)) j ++;
	for(; *(src + i); i ++, j ++)
		*(dst + j) = *(src + i);
	*(dst + j) = '\0';
	return dst;
  //panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
	int i = 0;
	while(*(s1 + i) && *(s1 + i) == *(s2 + i)) i ++;
	if(*(s1 + i) > *(s2 + i)) return 1;
	else if(*(s1 + i) < *(s2 + i)) return -1;
	else return 0;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
	for(int i = 0; i < n; i ++)
		*((int *)s + i) = c;
	return s;
  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
	int i = 0;
	while(*((char *)s1 + i) == *((char *)s2 + i) && i < n) i ++;
	if((*((char *)s1 + i)) > (*((char *)s2 + i)) && i != n) return 1;
	else if(*((char *)s1 + i) < (*((char *)s2 + i)) && i != n) return -1;
	else return 0;
  //panic("Not implemented");
}

#endif
