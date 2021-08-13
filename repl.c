#include <ctype.h>
#include <editline/history.h>
#include <editline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulisp.h"

lisp *parseAtom(char *s, int *i)
{
  char *astr = calloc(sizeof(char), 1);

  while (isalnum(s[*i])) {
    astr = realloc(astr, strlen(astr)+1);
    astr[strlen(astr)] = s[*i];
    astr[strlen(astr)+1] = '\0';
    (*i)++;
  }

  lisp *a = mkAtom(astr);
  free(astr);

  return a;
}

/* Parse an S-Expression */
lisp *parse(char *s, int *i) {
  if (*i < strlen(s)) {
    if (isalnum(s[*i])) {
      lisp *a = parseAtom(s, i);
      lisp *b = parse(s, i);
      return cons(a, b);
    } else if (s[*i] == '(') {
      ++*i;
      lisp *a = parse(s, i);
      ++*i;
      lisp *b = parse(s, i);
      return cons(a, b);
    } else if (s[*i] == ')') {
      return mkAtom("nil");
    } else if (s[*i] == ' ') {
      ++*i;
      return parse(s, i);
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }
}

int main(int argc, char **argv)
{
  fputs("µLisp v0.0.1\n", stderr);
  fputs("C-c to exit\n", stderr);

  while (1) {
    char *input = readline("> ");
    if (!input)
      break;

    add_history(input);

    lisp *l = NULL;
    int *i = calloc(1, sizeof(int));
    l = parse(input, i);

    l = eval(l, mkAtom("nil"));
    prettyPrint(l);
    puts("");

    free(i);
    freeLisp(l);
    free(input);
  }

  return 0;
}
