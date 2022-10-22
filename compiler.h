#pragma once

#define BRK asm volatile ("bkpt")
#define THUMB __attribute__((target("thumb")))
