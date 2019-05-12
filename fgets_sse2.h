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

#define SSE2_RESULTS  16 /* _mm_movemask_epi8 () returns a 16 bit int */
#define TABLE_CNT     (1 << SSE2_RESULTS)
#define BUFFER_EXTRA  1024

typedef struct
{
  uint8_t   len;
  uint8_t   lf;

} entry_t;

typedef struct
{
  uint32_t  len;
  uint32_t  off;

} next_t;

typedef struct
{
  uint8_t   cnt;
  entry_t   e[SSE2_RESULTS];

} table_t;

typedef struct
{
  int       size;
  char     *buf;
  next_t   *n;
  table_t  *t;

} ctx_t;

ctx_t *init_fgets_sse2 (int size);

next_t *fgets_sse2 (const ctx_t *ctx, FILE *stream);
