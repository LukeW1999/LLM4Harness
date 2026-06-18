/* native difftest stubs: debug/backtrace facilities unused in differential runs */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
void aws_backtrace_print(FILE *fp, void *call_site_data) { (void)fp; (void)call_site_data; }
void aws_debug_break(void) {}
void aws_secure_zero(void *pBuf, size_t bufsize) { memset(pBuf, 0, bufsize); }
