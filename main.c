#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <x86intrin.h>

#include "fgets_sse2.h"

#define BUFFER_SIZE (BUFSIZ * 32)

int main ()
{
  /* Our buffer to work with */

  ctx_t *ctx = init_fgets_sse2 (BUFFER_SIZE);

  /* Fire */

  next_t *ne;

  while ((ne = fgets_sse2 (ctx, stdin)) != NULL)
  {
    /* Demo1: Direct access, no data copied, for high performance processing

    printf ("%u %u\n", ne->off, ne->len);

    */

    /* Demo2: This is like like cat

    char *s = ctx->buf + ne->off;

    s[ne->len] = 0;

    puts (s);

    */
  }

  return 0;
}
