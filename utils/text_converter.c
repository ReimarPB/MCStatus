#include <uchar.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <iconv.h>
#endif

#include "../errors.h"

char *utf16be_to_utf8(char *text, size_t length)
{
#ifdef _WIN32

	wchar_t *wtext = (wchar_t *)text;
	for (int i = 0; i < wcslen(wtext); i++)
		wtext[i] = _byteswap_ushort(wtext[i]);

	char *result = malloc(length);
	memset(result, 0, length);

	WideCharToMultiByte(CP_UTF8, 0, wtext, length / 2, result, length, NULL, NULL);

	unsigned char *text2 = (unsigned char *)result;
	for (int i = 0; i < length; i++) {
		printf("%02x ", text2[i]);
	}

	return wtext;

#else

	iconv_t cd = iconv_open("UTF-8", "UTF-16BE");
	if (cd == (iconv_t)-1) system_error("Failed to convert unicode text");

	size_t in_length = length;
	size_t out_length = length;

	char result[length];
	memset(result, 0, length);

	char *outptr = (char*)result;
	if (iconv(cd, &text, &in_length, &outptr, &out_length) == -1)
		system_error("Failed to convert unicode text");

	iconv_close(cd);

	return strdup(result);

#endif
}

