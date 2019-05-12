#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE (BUFSIZ * 32)

int main ()
{
  char *buf = (char *) malloc (BUFFER_SIZE);

  size_t len = 0;

  char *ptr;

  while ((ptr = fgets (buf, BUFFER_SIZE, stdin)) != NULL)
  {
    len = strlen (ptr);
  }

  return len; /* anti-optimize */
}
