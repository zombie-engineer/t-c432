#pragma once

#define BRK asm volatile ("bkpt")
#define SECTION(__name) __attribute__((section(__name)))
#define PACKED __attribute__((packed))
#define THUMB __attribute__((target("thumb")))
#define PACKED __attribute__((packed))
