/*
 This code is copied from http://www.tux.org/pub/net/citadel/citadel/tools.c
 9-30-2003
 */

/*
 * decode_base64() and encode_base64() are adaptations of code by
 * John Walker, found in full in the file "base64.c" included with this
 * distribution.  The difference between those functions and these is that
 * these are intended to encode/decode small string buffers, and those are
 * intended to encode/decode entire MIME parts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef unsigned char byte;	      /* Byte type */
static byte dtable[256];	      /* base64 encode / decode table */

int encode_base64(char *dest, char *source)
{
    int i, hiteof = FALSE;
    int spos = 0;
    int dpos = 0;

    /*	Fill dtable with character encodings.  */

    for (i = 0; i < 26; i++) {
        dtable[i] = 'A' + i;
        dtable[26 + i] = 'a' + i;
    }
    for (i = 0; i < 10; i++) {
        dtable[52 + i] = '0' + i;
    }
    dtable[62] = '+';
    dtable[63] = '/';

    while (!hiteof) {
	    byte igroup[3], ogroup[4];
	    int c, n;

	    igroup[0] = igroup[1] = igroup[2] = 0;
	    for (n = 0; n < 3; n++) {
	      c = source[spos++];
	      if (c == 0) {
		      hiteof = TRUE;
		      break;
	      }
	      igroup[n] = (byte) c;
	    }
	    if (n > 0) {
	      ogroup[0] = dtable[igroup[0] >> 2];
	      ogroup[1] = dtable[((igroup[0] & 3) << 4) | (igroup[1] >> 4)];
	      ogroup[2] = dtable[((igroup[1] & 0xF) << 2) | (igroup[2] >> 6)];
	      ogroup[3] = dtable[igroup[2] & 0x3F];

        /* Replace characters in output stream with "=" pad
	       characters if fewer than three characters were
	       read from the end of the input stream. */

	      if (n < 3) {
          ogroup[3] = '=';
		      if (n < 2) {
            ogroup[2] = '=';
		      }
	      }
	      for (i = 0; i < 4; i++) {
		      dest[dpos++] = ogroup[i];
		      dest[dpos] = 0;
	      }
	    }
    }
    return dpos;
}


/* 
 * Convert base64-encoded to binary.  Returns the length of the decoded data.
 * It will stop after reading 'length' bytes.
 */
int decode_base64(char *dest, char *source, size_t length)
{
    int i, c;
    int dpos = 0;
    int spos = 0;

    for (i = 0; i < 255; i++) {
    	dtable[i] = 0x80;
    }
    for (i = 'A'; i <= 'Z'; i++) {
        dtable[i] = 0 + (i - 'A');
    }
    for (i = 'a'; i <= 'z'; i++) {
        dtable[i] = 26 + (i - 'a');
    }
    for (i = '0'; i <= '9'; i++) {
        dtable[i] = 52 + (i - '0');
    }
    dtable['+'] = 62;
    dtable['/'] = 63;
    dtable['='] = 0;

    /*CONSTANTCONDITION*/
    while (TRUE) {
	    byte a[4], b[4], o[3];

	    for (i = 0; i < 4; i++) {
	      if (spos >= length) {
		      return(dpos);
	      }
	      c = source[spos++];

	      if (c == 0) {
		      if (i > 0) {
		        return(dpos);
		      }
		      return(dpos);
	      }
	      if (dtable[c] & 0x80) {
		      /* Ignoring errors: discard invalid character. */
		      i--;
		      continue;
	      }
	      a[i] = (byte) c;
	      b[i] = (byte) dtable[c];
	    }
	    o[0] = (b[0] << 2) | (b[1] >> 4);
	    o[1] = (b[1] << 4) | (b[2] >> 2);
	    o[2] = (b[2] << 6) | b[3];
      i = a[2] == '=' ? 1 : (a[3] == '=' ? 2 : 3);
	    if (i>=1) dest[dpos++] = o[0];
	    if (i>=2) dest[dpos++] = o[1];
	    if (i>=3) dest[dpos++] = o[2];
	    dest[dpos] = 0;
	    if (i < 3) {
  	    return(dpos);
 	    }
    }
}
