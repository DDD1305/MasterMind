#ifndef JEU_H
#define JEU_H

extern int level;
extern int win;
extern char *sol;

char *init(int n);
char *check(char *seq, int n);

#endif