/* File: string.h
 *
 * Function delcarations for string.c
 */

#ifndef STRING_H
#define STRING_H

#include <stdlib.h> /* for strdup() */

/* Appends the string pointed to, by 'src' to the end of
   the string pointed to by 'dest'. */
char* strcat (char* dest, const char* src);

/* Searches for the first occurence of the character c
   (an unsigned char) in the string pointed to, by the
   argument 'str'. */
char* strchr (const char* str, int c);

/* Compares the string pointed to, by 'str1' to the string
   pointed to by 'str2'. */
int strcmp (const char* str1, const char* str2);

/* Compares at most the first 'n' bytes of 'str1' and 'str2'. */
int strncmp (const char* str1, const char* str2, size_t n);

/* Copies the string pointed to, by 'src' to 'dest'. */
char* strcpy (char* dest, const char* src);

/* Computes the length of the string str up to but not
   including the terminating null character. */
size_t strlen (const char* str);

/* Creates a duplicate of the string str */
/* char* strdup (const char* str); undefined reference to malloc */

/* Finds the first character in the string 'str1' that
   matches any character specified in 'str2'. */
char* strpbrk (const char* str1, const char* str2);

/* Calculates the length of the initial segment of 'str1'
   which consists entirely of characters in 'str2'. */
size_t strspn (const char* str1, const char* str2);

/* Finds the first occurence of the entire string 'needle'
   (not including the terminating null character) which
   appears in the string 'haystack'. */
char* strstr (const char* haystack, const char* needle);

/* Replace every character in str1 with c that is found in str2
 * until a character in str1 that is not in str2 is encountered.
 * Return first unmatching byte. 
 */
char* strrepl (char* str1, char c, const char* str2);

/* Breaks string str into a series of tokens separated by delim. */
char* strtok (char* str, const char* delim);


#endif	/* STRING_H */
