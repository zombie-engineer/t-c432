#pragma once
#define SVC_JUMP_TO_MAIN 55
#define SVC_PANIC        56
#define SVC_SLEEP        57

/*
 * svc_call macro uses preprocessor stringify feature
 * Additional intermediate macro expantion is needed to first expand the
 * MACRO-defined name of SVC argument into a numeric value 
 * Example: svc_call(JUMP_TO_MAIN) is first expanded into
 * __svc_call(55), which then uses stringification on a 55 instead of
 * JUMP_TO_MAIN to expand to  asm volatile("svc #55")
 */
#define __svc_call(__arg) asm volatile("svc #" #__arg)
#define svc_call(__arg) __svc_call(__arg)

int svc_handler(char arg);
