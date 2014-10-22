#ifndef	rfc2047_h
#define	rfc2047_h

/*
** Copyright 1998 - 2000 Double Precision, Inc.  See COPYING for
** distribution information.
*/


#ifdef  __cplusplus
extern "C" {
#endif


static const char rfc2047_h_rcsid[]="$Id: rfc2047.h,v 1.3 2000/09/29 04:38:47 mrsam Exp $";

extern int rfc2047_decode(const char *text,
		int (*func)(const char *, int, const char *, void *),
		void *arg);

extern char *rfc2047_decode_simple(const char *text);

extern char *rfc2047_decode_enhanced(const char *text, const char *mychset);

#define	RFC2047_DECODE_DISCARD	1
	/* options: Discard unknown charsets from decoded string. */
#define RFC2047_DECODE_ABORT	2
	/* options: Abort if we encounter an unknown charset, errno=EINVAL */




/*
** rfc2047_print is like rfc822_print, except that it converts RFC 2047
** MIME encoding to 8 bit text.
*/

struct rfc822a;

void rfc2047_print(const struct rfc822a *a,
	const char *charset,
	void (*print_func)(char, void *),
	void (*print_separator)(const char *, void *), void *);

/*
** And now, let's encode something with RFC 2047.  Encode the following
** string in the indicated character set, into a malloced buffer.  Returns 0
** if malloc failed.
*/

char *rfc2047_encode_str(const char *str, const char *charset);

/*
** If you can live with the encoded text being generated on the fly, use
** rfc2047_encode_callback, which calls a callback function, instead of
** dynamically allocating memory.
*/

int rfc2047_encode_callback(const char *str, const char *charset,
	int (*func)(const char *, size_t, void *), void *arg);

/*
** rfc2047_encode_header allocates a buffer, and MIME-encodes an RFC822 header
**
*/
char *rfc2047_encode_header(const struct rfc822a *a,
        const char *charset);



#ifdef  __cplusplus
}
#endif

#endif
