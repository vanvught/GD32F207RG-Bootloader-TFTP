/**
 * @file printf.c
 *
 */
/* Copyright (C) 2016-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

extern int console_putc(int);

struct context {
	int flag;
	int prec;
	int width;
	int total;
	int capacity;
};

enum {
	FLAG_PRECISION	    =	(1 << 0 ),
	FLAG_UPPERCASE	    =	(1 << 1 ),
	FLAG_LONG     	    =	(1 << 2 ),
	FLAG_NEGATIVE	    =	(1 << 3 ),
	FLAG_MIN_WIDTH	    =	(1 << 4 ),
	FLAG_ZERO_PADDED    =	(1 << 5 ),
	FLAG_LEFT_JUSTIFIED =	(1 << 6 )
};

static char *outptr = NULL;

inline static void _xputch(struct context *ctx, int c) {
	ctx->total++;

	if (outptr != NULL) {
		if (ctx->total < ctx->capacity) {
			*outptr++ = (char) c;
		}
		return;
	}

	console_putc(c);
}

static void _format_hex(struct context *ctx, unsigned int arg) {
	char buffer[64];
	char *p = buffer + (sizeof(buffer) / sizeof(buffer[0])) - 1;
	char *o = p;
	char alpha;
	char u;
	int i;

	if (arg == 0) {
		*p = '0';
		p--;
	} else {
		alpha = ((ctx->flag & FLAG_UPPERCASE) != 0) ? ((char) 'A' - (char) 10) : ((char) 'a' - (char) 10);

		do {
			u = (char) arg & (char) 0x0F;
			*p = (u < 10) ? (char)('0' + u) : (char)(alpha + u);
			p--;
			arg = arg >> 4;
		} while ((arg != 0) && (p > buffer));
	}

	if ((ctx->flag & FLAG_PRECISION) != 0) {
		while (((o - p) < ctx->prec) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_ZERO_PADDED) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_LEFT_JUSTIFIED) == 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = ' ';
		}
	}

	i = o - p;

	p++;

	while (p < buffer + (sizeof(buffer) / sizeof(buffer[0]))) {
		_xputch(ctx, (int) *p++);
	}

	while (i++ < ctx->width) {
		_xputch(ctx, (int) ' ');
	}
}

static void _format_int(struct context *ctx, uint64_t arg) {
	char buffer[64];
	char *p = buffer + (sizeof(buffer) / sizeof(buffer[0])) - 1;
	char *o = p;
	int i;
	
	if (arg == 0) {
		*p = '0';
		p--;
	} else {
		do {
			*p = (char)((arg % 10) + '0');
			p--;
			arg = arg / 10;
		} while ((arg != 0) && (p > buffer));
	}

	if ((ctx->flag & FLAG_PRECISION) != 0) {
		while (((o - p) < ctx->prec) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_ZERO_PADDED) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_NEGATIVE) != 0) {
		*p-- = '-';
	}

	if ((ctx->flag & FLAG_LEFT_JUSTIFIED) == 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = ' ';
		}
	}

	i = o - p;

	p++;

	while (p < buffer + (sizeof(buffer) / sizeof(buffer[0]))) {
		_xputch(ctx, (int) *p++);
	}

	while (i++ < ctx->width) {
		_xputch(ctx, (int) ' ');
	}
}

static void _format_string(struct context *ctx, const char *s) {
	int j;

	for (j = 0; s[j] != (char) 0; j++)
		;	// strlen

	if ((ctx->flag & FLAG_PRECISION) != 0) {
		if (ctx->prec < j) {
			j = ctx->prec;
		}
	}

	while ((((ctx->flag & FLAG_LEFT_JUSTIFIED) == 0)) && (j++ < ctx->width)) {
		_xputch(ctx, (int) ' ');
	}

	while ((((ctx->flag & FLAG_PRECISION) == 0) || (ctx->prec != 0)) && (*s != (char) 0)) {
		_xputch(ctx, (int) *s++);
		ctx->prec--;
	}

	while (j++ < ctx->width) {
		_xputch(ctx, (int) ' ');
	}
}

static void _format_pointer(struct context *ctx, unsigned int arg) {
	ctx->width = 8;
	ctx->flag = FLAG_ZERO_PADDED;

	_xputch(ctx, (int) '0');
	_xputch(ctx, (int) 'x');

	_format_hex(ctx, arg);
}

static int _vprintf(const int size, const char *fmt, va_list va) {
	struct context ctx;
	float f;
	int64_t l;
	uint64_t lu;
	const char *s;

	ctx.total = 0;
	ctx.capacity = size;

	while (*fmt != (char) 0) {

		if (*fmt != '%') {
			_xputch(&ctx, (int )*fmt++);
			continue;
		}

		fmt++;

		ctx.flag = 0;
		ctx.prec = 0;
		ctx.width = 0;

		if (*fmt == '0') {
			ctx.flag |= FLAG_ZERO_PADDED;
			fmt++;
		} else if (*fmt == '-') {
			ctx.flag |= FLAG_LEFT_JUSTIFIED;
			fmt++;
		}

		while (isdigit((int) *fmt) != 0) {
			ctx.width = ctx.width * 10 + (int) (*fmt - '0');
			fmt++;
		}

		if (ctx.width != 0) {
			ctx.flag |= FLAG_MIN_WIDTH;
		}

		if (*fmt == '.') {
			fmt++;
			if (*fmt == '*') {
				fmt++;
				ctx.prec = (int) va_arg(va, int);
				if (ctx.prec < 0) {
					ctx.prec = -ctx.prec;
				}
			} else {
				while (isdigit((int) *fmt) != 0) {
					ctx.prec = ctx.prec * 10 + (int) (*fmt - '0');
					fmt++;
				}
			}
			ctx.flag |= FLAG_PRECISION;
		}

		if (*fmt == 'l') {
			fmt++;
			ctx.flag |= FLAG_LONG;
		}

		switch (*fmt) {
		case 'c':
			_xputch(&ctx, va_arg(va, int));
			break;
		case 'd':
			/*@fallthrough@*/
			/* no break */
		case 'i':
			l = ((ctx.flag & FLAG_LONG) != 0) ? va_arg(va, int64_t) : (int64_t) va_arg(va, int32_t);
			if (l < 0) {
				ctx.flag |= FLAG_NEGATIVE;
				l = -l;
			}
			_format_int(&ctx, (uint64_t) l);
			break;
		case 'p':
			_format_pointer(&ctx, va_arg(va, uint32_t));
			break;
		case 's':
			s = va_arg(va, const char *);
			_format_string(&ctx, s);
			break;
		case 'u':
			lu = ((ctx.flag & FLAG_LONG) != 0) ? va_arg(va, uint64_t) : va_arg(va,uint32_t);
			_format_int(&ctx, lu);
			break;
		case 'X':
			ctx.flag |= FLAG_UPPERCASE;
			/*@fallthrough@*/
			/* no break */
		case 'x':
			_format_hex(&ctx, va_arg(va, uint32_t));
			break;
		default:
			_xputch(&ctx, (int) *fmt);
			continue;
		}
		
		fmt++;
	}

	return ctx.total;
}

int printf(const char* fmt, ...) {
	int i;
	va_list arp;

	va_start(arp, fmt);

	i = _vprintf(INT_MAX, fmt, arp);

	va_end(arp);

	return i;
}

int vprintf(const char *fmt, va_list arp) {
	int i;

	i = _vprintf(INT_MAX, fmt, arp);

	return i;
}

int sprintf(char *str, const char *fmt, ...) {
	int i;
	va_list arp;

	outptr = str;
	va_start(arp, fmt);

	i = _vprintf(INT_MAX, fmt, arp);

	va_end(arp);

	*outptr = (char) 0;
	outptr = NULL;

	return i;
}

int vsprintf(char *str, const char *fmt, va_list ap) {
	int i;

	outptr = str;

	i = _vprintf(INT_MAX, fmt, ap);

	*outptr = (char) 0;
	outptr = NULL;

	return i;
}

int snprintf(char *str, size_t size, const char *fmt, ...) {
	int i;
	va_list arp;

	outptr = str;
	va_start(arp, fmt);

	i = _vprintf((int) size, fmt, arp);

	va_end(arp);

	*outptr = (char) 0;
	outptr = NULL;

	return i;

}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
	int i;

	outptr = str;

	i = _vprintf((int) size, fmt, ap);

	*outptr = (char) 0;
	outptr = NULL;

	return i;
}
