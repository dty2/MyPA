#include <am.h>
#include <nemu.h>
#include <stdio.h>

static uint64_t boottime = 0;

uint64_t gettime()
{
	uint64_t time1 = (uint64_t)inl(AUDIO_ADDR);
	uint64_t time2 = (uint64_t)inl(AUDIO_ADDR + 8) << 32;
	return time2 | time1;
}

void __am_timer_init() {
	boottime = gettime();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	uptime->us = gettime() - boottime;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
