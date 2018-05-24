/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string_ext.h>
#include <trace.h>
#include <util.h>
#include <utee_syscalls.h>

#if TRACE_LEVEL > 0

void trace_ext_puts(const char *str)
{
	utee_log(str, strlen(str));
}

int trace_ext_get_thread_id(void)
{
	return -1;
}


/*
 * printf and puts - stdio printf support
 *
 * 'printf()' and 'puts()' traces have the 'info' trace level.
 * Traces are prefixed with string "[ta log] ".
 */
int printf(const char *fmt, ...)
{
	char to_format[MAX_PRINT_SIZE];
	static const char failed[] = "printf failed\n";
	va_list ap;
	int s;

	va_start(ap, fmt);
	s = vsnprintf(to_format, sizeof(to_format), fmt, ap);
	va_end(ap);

	if (s < 0) {
		trace_ext_puts(failed);
		return s;
	}

	trace_ext_puts(to_format);

	return s;
}

int puts(const char *str)
{
	if (trace_get_level() >= TRACE_PRINTF_LEVEL)
		trace_ext_puts(str);
	return 1;
}

#else

int printf(const char *fmt __unused, ...)
{
	return 0;
}

int puts(const char *str __unused)
{
	return 0;
}

#endif

static int
__flexsc_vsnprintf(char *s_buf __unused, size_t s_size __unused,
					const char *fmt __unused, va_list ap __unused) {
    return 1;
}

void
flexsc_debug_nolock(const char *fmt, ...) {
    char s_buf[4096];
    va_list arg;
    va_start(arg, fmt);
    __flexsc_vsnprintf(s_buf, sizeof(s_buf), fmt, arg);
    va_end(arg);
}
