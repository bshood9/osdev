/* File: string.c
 *
 * String manipulation library
 */

#include "string.h"

// TODO: make token a static variable in strtok()
char* token = NULL;

char* strcat (char* dest, const char* src)
{
  char* dest0 = dest;

  while (*dest)
   ++dest;

  while (*dest++ = *src++);

  return dest0;
}

char* strchr (const char* str, int c)
{
  while (*str)
    {
      if (*str == c)
	return (char*)str;

      ++str;
    }

  return NULL;
}

int strcmp (const char* str1, const char* str2)
{
  /* "If both strings are not null-terminated (and identical), what happens??"

     Iteration vs. Recursion:     
     Get stuck in an infinite loop vs. crash the program (stack overflow) */

  if (*str1 && *str2 && (*str1 == *str2))
    {
      return strcmp(++str1, ++str2);
    }
  
  // TODO: the real strcmp() returns the difference
  if (*str1 - *str2 > 0) return 1;
  else if (*str1 - *str2 < 0) return -1;
  else return 0;
}

int strncmp (const char* str1, const char* str2, size_t n)
{
  if (*str1 && *str2 && (*str1 == *str2) && --n)
    {
      strncmp(++str1, ++str2, --n);
    }

  if (*str1 - *str2 > 0) return 1;
  else if (*str1 - *str2 < 0) return -1;
  else return 0;
}

char* strcpy (char* dest, const char* src)
{
  char* dest0 = dest;

  while (*dest++ = *src++);

  return dest0;
}

size_t strlen (const char* str)
{
  size_t n = 0;

  while (*str++)
    ++n;

  return n;
}
/* UNDEFINED REFERENCE TO MALLOC
char* strdup (const char* str)
{
  char* new = malloc(strlen(str) + 1);

  return strcpy(new, str);
}
*/
char* strpbrk (const char* str1, const char* str2)
{
  if (str1 && *str1)
    {
      if (strchr(str2, *str1))
	return (char*)str1;

      else
	return strpbrk(++str1, str2);
    }

  return NULL;
}

size_t strspn (const char* str1, const char* str2)
{
  size_t count = 0;

  while (strchr(str2, *str1))
    {
      ++count;
      ++str1;
    }

  return count;
}

char* strstr (const char* haystack, const char* needle)
{
  const char* string;
  const char* substring;
  const char* substr_0;

  /* no protection for null input pointers */
  /* this does not account for the case where the beginning */
  /* of a pattern is detected, but the real pattern begins */
  /* within there. e.g. if you are looking for pattern 'aabc' in */
  /* string 'aaabc', then you will start to match at index 0, */
  /* no longer match at index 2, but not find that the pattern is */
  /* at index 1 */
  while (*(substr_0 = string = haystack++) != '\0') /* added dereference */
    {      
      substring = needle;

      while (*string && *substring && (*string++ == *substring++));
      
      if (!(*substring))
	return (char*)substr_0;
    }

  return NULL;
}

/* Replace every character in str1 with c that is found in str2
 * until a character in str1 that is not in str2 is encountered
 */
char* strrepl (char* str1, char c, const char* str2)
{
  while (str1 && *str1 && strchr(str2, *str1))
    {
      *str1 = c;
      ++str1;
    }

  return str1;			/* Return first byte from str1 not in str2 */
}

char* strtok (char* str, const char* delim)
{
  char* nxt_delim;

  if (!str)
    {
      str = token;
      /* TODO: move token declaration to this function as a static */
    }
  
  nxt_delim = strpbrk(str, delim); /* get pointer to next delimiter */

  /* first time through, make recursive call if the start of the string is
   * one or more delimiter characters
   */
  if (str && (str == nxt_delim))
    {
      return strtok(strrepl(nxt_delim, '\0', delim), delim);
    }

  /* overwrite contiguous delimiter bytes */
  nxt_delim = strrepl(nxt_delim, '\0', delim);

  token = nxt_delim; /* set token to start of next token */

  return str;
}

