#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT)-boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
	//uint32_t * p=(uint32*)0x40000;
	
	/*while(x<400&&x>0&&y<300&&y>0&&w<32&&w>0){
					for(int i=0;i<h;i++){
									//memcpy();
					}
	}*/
}

void _draw_sync() {
}

int _read_key() {
	if(inb(0x64)==1) return inl(0x60);
	else	return _KEY_NONE;
}
