/* string.h */

extern int strlen(const char *str);
extern int strnlen(const char *, uint);

extern int strcmp(char *, char *);
extern int strncmp(const char *, const char *, int);

extern char* strcpy(char *tar, const char *src);
extern char* strncpy(char *, const char *, int);

extern char* strcat(char *, const char *);
extern char* strncat(char *, const char *, int);

extern char* strchr(const char *, int);
extern char* strrchr(const char *, int);

extern char* strstr(const char *, const char *);

extern int strspn(const char *s, const char *accept);
extern char* strpbrk(const char *s, const char *accept);
extern char* strtok(char *s, const char *delim);

