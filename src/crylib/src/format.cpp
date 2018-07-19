// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <cstdio>

#if 0

int strformat(VMHANDLE v, int nformatstringidx, int *outlen, const char **output) {
	const char *format;
	char *dest;
	char fmt[MAX_FORMAT_LEN];
	lv_getstring(v, nformatstringidx, &format);
	LVInteger format_size = lv_getsize(v, nformatstringidx);
	LVInteger allocated = (format_size + 2) * sizeof(LVChar);
	dest = lv_getscratchpad(v, allocated);
	LVInteger n = 0, i = 0, nparam = nformatstringidx + 1, w = 0;

	//while(format[n] != '\0')
	while (n < format_size) {
		if (format[n] != '%') {
			assert(i < allocated);
			dest[i++] = format[n];
			n++;
		}
		else if (format[n + 1] == '%') { //handles %%
			dest[i++] = '%';
			n += 2;
		}
		else {
			n++;
			if (nparam > lv_gettop(v))
				return lv_throwerror(v, _LC("not enough paramters for the given format string"));
			n = validate_format(v, fmt, format, n, w);
			if (n < 0)
				return -1;
			LVInteger addlen = 0;
			LVInteger valtype = 0;
			const LVChar *ts = NULL;
			LVInteger ti = 0;
			LVFloat tf = 0;
			switch (format[n]) {
			case 's':
				if (LV_FAILED(lv_getstring(v, nparam, &ts)))
					return lv_throwerror(v, _LC("string expected for the specified format"));
				addlen = (lv_getsize(v, nparam) * sizeof(LVChar)) + ((w + 1) * sizeof(LVChar));
				valtype = 's';
				break;
			case 'i':
			case 'd':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
#ifdef _LV64
			{
				size_t flen = scstrlen(fmt);
				LVInteger fpos = flen - 1;
				LVChar f = fmt[fpos];
				const LVChar *prec = (const LVChar *)_PRINT_INT_PREC;
				while (*prec != _LC('\0')) {
					fmt[fpos++] = *prec++;
				}
				fmt[fpos++] = f;
				fmt[fpos++] = _LC('\0');
			}
#endif
			case 'c':
				if (LV_FAILED(lv_getinteger(v, nparam, &ti)))
					return lv_throwerror(v, _LC("integer expected for the specified format"));
				addlen = (ADDITIONAL_FORMAT_SPACE)+((w + 1) * sizeof(LVChar));
				valtype = 'i';
				break;
			case 'f':
			case 'g':
			case 'G':
			case 'e':
			case 'E':
				if (LV_FAILED(lv_getfloat(v, nparam, &tf)))
					return lv_throwerror(v, _LC("float expected for the specified format"));
				addlen = (ADDITIONAL_FORMAT_SPACE)+((w + 1) * sizeof(LVChar));
				valtype = 'f';
				break;
			default:
				return lv_throwerror(v, _LC("invalid format"));
			}
			n++;
			allocated += addlen + sizeof(LVChar);
			dest = lv_getscratchpad(v, allocated);
			switch (valtype) {
			case 's':
				i += scsprintf(&dest[i], allocated, fmt, ts);
				break;
			case 'i':
				i += scsprintf(&dest[i], allocated, fmt, ti);
				break;
			case 'f':
				i += scsprintf(&dest[i], allocated, fmt, tf);
				break;
			};
			nparam++;
		}
	}
	*outlen = i;
	dest[i] = '\0';
	*output = dest;
	return LV_OK;
}

#endif
