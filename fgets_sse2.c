/*
 * fgets_sse2 - SSE2 accelerated fgets(3) replacement
 *
 * Copyright (C) 2012 Jens Steube <jens.steube@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

#include "fgets_sse2.h"

ctx_t *init_fgets_sse2 (int size)
{
  ctx_t *ctx = (ctx_t *) malloc (sizeof (ctx_t));

  ctx->size = size;

  table_t *t = (table_t *) calloc (TABLE_CNT, sizeof (table_t));

  ctx->t = t;

  int key;

  for (key = 0; key < TABLE_CNT; key++)
  {
    t->cnt = 0;

    entry_t *e = t->e;

    e->lf = 0;

    e->len = 0;

    int i;

    for (i = 0; i < SSE2_RESULTS; i++)
    {
      if ((key >> i) & 1)
      {
        t->cnt++;

        e->lf = 1;

        e++;

        e->lf = 0;

        e->len = 0;
      }
      else
      {
        e->len++;
      }
    }

    t++;
  }

  next_t *n = (next_t *) calloc (size, sizeof (next_t));

  ctx->n = n;

  char *buf = (char *) _mm_malloc (size, 16);

  ctx->buf = buf;

  return ctx;
}

next_t *generate_nws (entry_t *e, uint8_t cnt, next_t *n)
{
  uint16_t len = n->len;

  uint8_t i;

  for (i = 0; i < cnt; i++) /* This key has more than the default entry */
  {
    len += e->len;

    n->len = len;

    n++;

    len = 0;

    e++;
  }

  len += e->len;

  if (e->lf)
  {
    n->len = len;

    n++;

    len = 0;
  }

  n->len = len;

  return n;
}

next_t *fgets_sse2 (const ctx_t *ctx, FILE *stream)
{
  static next_t *cur  = NULL;
  static next_t *last = NULL;

  if (cur < last)
  {
    next_t *prev = cur;

    cur++;

    cur->off = prev->off + prev->len + 1; /* + 1 for the \n */

    return prev;
  }

  /* Buffer is empty, we need to reload */

  size_t num = fread (ctx->buf, 1, ctx->size - BUFFER_EXTRA - 15, stream);

  if (!num) return NULL;

  /* Ensure buffer ends with a LF - makes our live much easier */

  size_t i;

  for (i = 0; i < BUFFER_EXTRA; i++, num++)
  {
    if (ctx->buf[num - 1] == '\n') break;

    const int c = fgetc (stream);

    if (c == EOF) break;

    ctx->buf[num] = c;
  }

  /* Overlapping data from previous runs need to be cleared */

  memset (ctx->buf + num, 0, 15);

  /* Reset pointers and stuff */

  ctx->n->len = 0;

  cur = ctx->n;

  last = ctx->n;

  /* Start processing */

  __m128i mask = _mm_set1_epi32 (0x0a0a0a0a);

  __m128i *ptr = (__m128i *) ctx->buf;

  for (i = 0; i < num; i += 16)
  {
    __m128i tmp = _mm_cmpeq_epi8 (mask, *ptr++);

    const int k = _mm_movemask_epi8 (tmp);

    table_t *te = &ctx->t[k];

    last = generate_nws (te->e, te->cnt, last);
  }

  /* A single recursion to save some code */

  return fgets_sse2 (ctx, stream);
}
