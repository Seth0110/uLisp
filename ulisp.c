/* Core Scheme implementation built using SICP
 *
 * This file is part of µLisp.
 *
 * µLisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * µLisp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with µLisp.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ulisp.h"

#define DEBUG 0

/* Naming conversions from SICP:
 * null?           -> is_null
 * primitive-apply -> primitive_apply
 * set-car!        -> set_car_i
 */

void s(lisp *l);

void freeLisp (lisp *l)
{
  if (l) {
    if (l->atom)
      free(l->atom);
    if (l->fst)
      freeLisp(l->fst);
    if (l->snd)
      freeLisp(l->snd);
    free(l);
  }
}

lisp *copy(lisp *l)
{
  if (!l)
    return NULL;
  if (l->atom)
    return mkAtom(l->atom);
  else
    return cons(copy(car(l)),
		copy(cdr(l)));
}

lisp *cons(lisp *a, lisp *b)
{
  lisp *l;
  if (!a) {
    l = b;
  } else if (!b) {
    l = a;
  } else {
    l = malloc(sizeof(lisp));
    l->atom = NULL;
    l->fst = a;
    l->snd = b;
  }
  return l;
}

lisp *append(lisp *list1, lisp *list2)
{
  if (!list1)
    return list2;
  else
    return cons(car(list1), append(cdr(list1), list2));
}

lisp *list(int argc, ...)
{
  lisp *l = mkAtom("nil");
  lisp **ls = malloc(sizeof(lisp *) * argc);
  va_list valist;
  va_start(valist, argc);

  for (int i = 0; i < argc; i++)
    ls[i] = va_arg(valist, lisp *);

  for (int i = argc-1; i >= 0; i--)
    l = cons(ls[i], l);

  va_end(valist);
  free(ls);

  return l;
}

lisp *to_list(lisp *l)
{
  if (is_null(l))
    return mkAtom("nil");
  else
    return cons(car(l),
		to_list(cdr(l)));
}

lisp *primitive_procedures_list()
{
  return list(24,
	      mkAtom("car"),
	      mkAtom("cdr"),
	      mkAtom("cons"),
	      mkAtom("atom?"),
	      mkAtom("eq?"),
	      mkAtom("equal?"),
	      mkAtom("null?"), // Move to base.scm?
	      mkAtom("+"),
	      mkAtom("-"),
	      mkAtom("*"),
	      mkAtom("/"),
	      mkAtom("remainder"),
	      mkAtom("="),
	      mkAtom(">"),
	      mkAtom("<"),
	      mkAtom("list"), // Move to base.scm?
	      mkAtom("not"),
	      mkAtom("and"),
	      mkAtom("or"),
	      mkAtom("sin"),
	      mkAtom("cos"),
	      mkAtom("display"),
	      mkAtom("error"),
	      mkAtom("random")
	      );
}

lisp *car(lisp *l)
{
  return l->fst;
}

lisp *safe_car(lisp *l)
{
  if (l && l->fst) {
    return l->fst;
  } else {
    puts("Argument of incorrect type supplied to: car");
    return mkAtom("ERR");
  }
}

void set_car_i(lisp *x, lisp *y)
{
  *x = *(cons(y, cdr(x)));
}

void set_cdr_i(lisp *x, lisp *y)
{
  *x = *(cons(car(x), y));
}

int is_eq_str(lisp *a, char *b)
{
  lisp *b_atom = mkAtom(b);
  int result = is_eq(a, b_atom);
  freeLisp(b_atom);
  return result;
}

int is_eq(lisp *a, lisp *b)
{
  if (a && is_atom(a) && b && is_atom(b))
    return !strcmp(a->atom, b->atom);
  else
    return 0;
}

int is_null(lisp *l)
{
  return is_eq_str(l, "nil");
}

lisp *cdr(lisp *l)
{
  return l->snd;
}

lisp *safe_cdr(lisp *l)
{
  if (l && l->snd) {
    return l->snd;
  } else {
    puts("Argument of incorrect type supplied to: cdr");
    return mkAtom("ERR");
  }
}

lisp *mkAtom(char *val)
{
  lisp *l = malloc(sizeof(lisp));
  l->atom = malloc(sizeof(char)*(strlen(val)+1));
  l->fst = NULL;
  l->snd = NULL;
  strcpy(l->atom, val);
  return l;
}

lisp *mkStrAtom(char *val)
{
  int len = strlen(val);
  char *str = malloc(sizeof(char) * (len + 3));

  str[0] = '"';
  for (int i = 0; i < len+1; i++)
    str[i+1] = val[i];
  str[len+1] = '"';
  str[len+2] = '\0';

  lisp *out = mkAtom(str);
  free(str);

  return out;
}

int atomcmp(lisp *l, char *s)
{
  return strcmp(l->atom, s);
}

int is_false(lisp *x)
{
  return is_eq_str(x, "#f");
}

int is_true(lisp *x)
{
  return !is_false(x);
}

double ltof(lisp *l)
{
  return atof(l->atom);
}

int is_atom(lisp *l)
{
  return l != NULL && l->atom != NULL;
}

lisp *caar(lisp *l)
{
  return car(car(l));
}

lisp *cadr(lisp *l)
{
  return car(cdr(l));
}

lisp *cdar(lisp *l)
{
  return cdr(car(l));
}

lisp *cddr(lisp *l)
{
  return cdr(cdr(l));
}

lisp *cadar(lisp *l)
{
  return car(cdr(car(l)));
}

lisp *caadr(lisp *l)
{
  return car(car(cdr(l)));
}

lisp *caddr(lisp *l)
{
  return car(cdr(cdr(l)));
}

lisp *cdadr(lisp *l)
{
  return cdr(car(cdr(l)));
}

lisp *cadddr(lisp *l)
{
  return car(cdr(cdr(cdr(l))));
}

lisp *cdddr(lisp *l)
{
  return cdr(cdr(cdr(l)));
}

int is_equal(lisp *x, lisp *y)
{
  if (is_atom(x) && is_atom(y))
    return is_eq(x, y);
  else if (is_equal(car(x), car(y)))
    return is_equal(cdr(x), cdr(y));
  else
    return 0;
}

lisp *pairlis(lisp *x, lisp *y, lisp *a)
{
  if (!x)
    return a;
  else
    return cons(cons(car(x), car(y)),
		pairlis(cdr(x), cdr(y), a));
}

lisp *assoc(lisp *x, lisp *a)
{
  if (!a)
    fprintf(stderr, "Undefined association: %s\n", x->atom);
  if (is_equal(caar(a), x))
    return car(a);
  else
    return assoc(x, cdr(a));
}

lisp *error(char *msg, int lispc, ...)
{
  fprintf(stderr, "%s: ", msg);

  va_list valist;
  lisp *l;
  va_start(valist, lispc);
  for (int i = 0; i < lispc; i++) {
    l = va_arg(valist, lisp *);
    if (!is_null(l)) {
      prettyPrint(l);
      puts("");
    }
  }

  va_end(valist);

  return mkAtom("ERR");
}

lisp *error_lisp(lisp *e)
{
  if (is_null(e)) {
    return mkAtom("ERR");
  } else if (length(e) == 1) {
    display(car(e));
    puts("");
    return error_lisp(cdr(e));
  } else {
    display(car(e));
    printf(" ");
    return error_lisp(cdr(e));
  }
}

int is_last_exp(lisp *seq)
{
  return is_null(cdr(seq));
}

lisp *first_exp(lisp *seq)
{
  return car(seq);
}

lisp *rest_exps(lisp *seq)
{
  return cdr(seq);
}

int is_pair(lisp *seq)
{
  return length(seq) > 1;
}

int is_tagged_list(lisp *exp, char *tag)
{
  return is_pair(exp) && is_eq_str(car(exp), tag);
}

int is_if(lisp *exp)
{
  return is_tagged_list(exp, "if");
}

lisp *if_predicate(lisp *exp)
{
  return cadr(exp);
}

lisp *if_consequent(lisp *exp)
{
  return caddr(exp);
}

lisp *if_alternative(lisp *exp)
{
  if (!is_null(cdddr(exp)))
    return cadddr(exp);
  else
    return mkAtom("#f");
}

lisp *make_if(lisp *predicate, lisp *consequent, lisp *alternative)
{
  return list(4, mkAtom("if"), predicate, consequent, alternative);
}

lisp *eval_if(lisp *exp, lisp *env)
{
  if (is_true(eval(if_predicate(exp), env)))
    return eval(if_consequent(exp), env);
  else
    return eval(if_alternative(exp), env);
}

lisp *eval_sequence(lisp *exps, lisp *env)
{
  if (is_last_exp(exps)) {
    return eval(first_exp(exps), env);
  } else {
    eval(first_exp(exps), env);
    return eval_sequence(rest_exps(exps), env);
  }
}

lisp *eval_definition(lisp *exp, lisp *env);

int is_elem(lisp *a, lisp *l)
{
  if (!l || !car(l))
    return 0;
  else if (is_atom(car(l)))
    return is_eq(a, car(l)) || is_elem(a, cdr(l));
  else
    return is_elem(a, cdr(l));
}

int is_primitive_procedure(lisp *p)
{
  lisp *prims = primitive_procedures_list();
  int result = is_elem(p, prims);
  freeLisp(prims);
  return result;
}

lisp *the_empty_environment()
{
  return mkAtom("nil");
}

lisp *setup_environment()
{
  lisp *initial_env = the_empty_environment();
  initial_env = extend_environment(list(2, mkAtom("true"), mkAtom("false")), list(2, mkAtom("#t"), mkAtom("#f")), initial_env);
  return initial_env;
}

lisp *the_global_environment()
{
  return setup_environment();
}

int is_compound_procedure(lisp *p)
{
  return is_tagged_list(p, "procedure");
}

lisp *make_procedure(lisp *parameters, lisp *body, lisp *env)
{
  return list(4, mkAtom("procedure"), parameters, body, env);
}

lisp *procedure_parameters(lisp *p)
{
  return cadr(p);
}

lisp *procedure_body(lisp *p)
{
  return caddr(p);
}

lisp *procedure_environment(lisp *p)
{
  return cadddr(p);
}

double plus(lisp *l)
{
  if (is_null(l)) {
    return 0;
  } else if (is_number(car(l))) {
    return ltof(car(l)) + plus(cdr(l));
  } else {
    fputs("Not a number -- PLUS\n", stderr);
    prettyPrint(l);
    puts("");
    return 0;
  }
}

double subtract(lisp *l)
{
  if (is_null(l))
    return 0;
  else
    return ltof(car(l)) - subtract(cdr(l));
}

double minus(lisp *l)
{
  if (length(l) == 1)
    return 0 - ltof(car(l));
  else
    return subtract(l);
}

double multiply(lisp *l)
{
  if (is_null(l))
    return 1;
  else
    return ltof(car(l)) * multiply(cdr(l));
}

double divide(lisp *l)
{
  if (is_null(l))
    return 1;
  else
    return ltof(car(l)) / divide(cdr(l));
}

double rem(lisp *x, lisp *y)
{
  return fmod(ltof(x), ltof(y));
}

double num_sin(lisp *x)
{
  return sin(ltof(x));
}

double num_cos(lisp *x)
{
  return cos(ltof(x));
}

lisp *not(lisp *x)
{
  if (is_false(car(x)))
    return mkAtom("#t");
  else
    return mkAtom("#f");
}

lisp *and(lisp *xs)
{
  if (is_null(xs))
    return mkAtom("#t");
  else if (is_true(car(xs)))
    return and(cdr(xs));
  else
    return mkAtom("#f");
}

lisp *or(lisp *xs)
{
  if (is_null(xs))
    return mkAtom("#f");
  else if (is_true(car(xs)))
    return mkAtom("#t");
  else
    return or(cdr(xs));
}

lisp *num_eq(lisp *l)
{
  int val = ltof(car(l));
  int i = 1;
  while (i < length(l)) {
    if (ltof(nth(l, i)) != val)
      return mkAtom("#f");
    i++;
  }
  return mkAtom("#t");
}

lisp *num_gt(lisp *l)
{
  int val = ltof(car(l));
  int i = 1;
  while (i < length(l)) {
    if (ltof(nth(l, i)) >= val)
      return mkAtom("#f");
    val = ltof(nth(l, i));
    i++;
  }
  return mkAtom("#t");
}

lisp *num_lt(lisp *l)
{
  int val = ltof(car(l));
  int i = 1;
  while (i < length(l)) {
    if (ltof(nth(l, i)) <= val)
      return mkAtom("#f");
    val = ltof(nth(l, i));
    i++;
  }
  return mkAtom("#t");
}

lisp *ftol(double f)
{
  char s[16];
  sprintf(s, "%g", f);
  return mkAtom(s);
}

lisp *btol(int b)
{
  if (b)
    return mkAtom("#t");
  else
    return mkAtom("#f");
}

lisp *apply_primitive_procedure(lisp *procedure, lisp *arguments)
{
  if (is_eq_str(procedure, "car"))
    return safe_car(car(arguments));
  else if (is_eq_str(procedure, "cdr"))
    return safe_cdr(car(arguments));
  else if (is_eq_str(procedure, "cons"))
    return cons(car(arguments), cadr(arguments));
  else if (is_eq_str(procedure, "atom?"))
    return btol(is_atom(car(arguments)));
  else if (is_eq_str(procedure, "eq?"))
    return btol(is_eq(car(arguments), cadr(arguments)));
  else if (is_eq_str(procedure, "equal?"))
    return btol(is_equal(car(arguments), cadr(arguments)));
  else if (is_eq_str(procedure, "null?"))
    return btol(is_null(car(arguments)));
  else if (is_eq_str(procedure, "+"))
    return ftol(plus(arguments));
  else if (is_eq_str(procedure, "-"))
    return ftol(minus(arguments));
  else if (is_eq_str(procedure, "*"))
    return ftol(multiply(arguments));
  else if (is_eq_str(procedure, "/"))
    return ftol(divide(arguments));
  else if (is_eq_str(procedure, "remainder"))
    return ftol(rem(car(arguments), cadr(arguments)));
  else if (is_eq_str(procedure, "="))
    return num_eq(arguments);
  else if (is_eq_str(procedure, ">"))
    return num_gt(arguments);
  else if (is_eq_str(procedure, "<"))
    return num_lt(arguments);
  else if (is_eq_str(procedure, "list"))
    return to_list(arguments);
  else if (is_eq_str(procedure, "not"))
    return not(arguments);
  else if (is_eq_str(procedure, "and"))
    return and(arguments);
  else if (is_eq_str(procedure, "or"))
    return or(arguments);
  else if (is_eq_str(procedure, "sin"))
    return ftol(num_sin(car(arguments)));
  else if (is_eq_str(procedure, "cos"))
    return ftol(num_cos(car(arguments)));
  else if (is_eq_str(procedure, "pair?"))
    return btol(is_pair(car(arguments)));
  else if (is_eq_str(procedure, "display"))
    return display(car(arguments));
  else if (is_eq_str(procedure, "error"))
    return error_lisp(arguments);
  else if (is_eq_str(procedure, "random"))
    return random(car(arguments));
  else
    return error("Tried to apply non-primitive procedure", 2, procedure, arguments);
}

lisp *make_frame(lisp *variables, lisp *values)
{
  return cons(variables, values);
}

lisp *frame_variables(lisp *frame)
{
  return car(frame);
}

lisp *frame_values(lisp *frame)
{
  return cdr(frame);
}

void add_binding_to_frame_i(lisp *var, lisp *val, lisp *frame)
{
  set_car_i(frame, cons(var, car(frame)));
  set_cdr_i(frame, cons(val, cdr(frame)));
}

lisp *extend_environment(lisp *vars, lisp *vals, lisp *base_env)
{
  if (length(vars) == length(vals))
    return cons(make_frame(vars, vals), base_env);
  else
    if (length(vars) < length(vals))
      return error("Too many arguments supplied", 2, vars, vals);
    else
      return error("Too few arguments supplied", 2, vars, vals);
}

lisp *enclosing_environment(lisp *env)
{
  return cdr(env);
}

lisp *first_frame(lisp *env)
{
  return car(env);
}

lisp *lookup_variable_value(lisp *var, lisp *env)
{
  return env_loop(var, env);
}

lisp *env_loop(lisp *var, lisp *env)
{
  if (is_eq(env, the_empty_environment())) {
    return error("Unbound variable", 1, var);
  } else {
    lisp *frame = first_frame(env);
    return env_scan(var,
		    frame_variables(frame),
		    frame_values(frame),
		    env);
  }
}

lisp *env_scan(lisp *var, lisp *vars, lisp *vals, lisp *env)
{
  if (is_null(vars)) {
    return env_loop(var, enclosing_environment(env));
  } else if (is_eq(var, car(vars))) {
    return car(vals);
  } else {
    return env_scan(var, cdr(vars), cdr(vals), env);
  }
}

lisp *apply(lisp *procedure, lisp *arguments)
{
  if (DEBUG) {
    printf("apply : ");
    prettyPrint(procedure);
    printf("\n      : ");
    prettyPrint(arguments);
    puts("");
  }
  if (is_err(procedure) || is_err(arguments))
    return mkAtom("ERR");
  else if (is_primitive_procedure(procedure))
    return apply_primitive_procedure(procedure, arguments);
  else if (is_compound_procedure(procedure)) {
    return eval_sequence(procedure_body(procedure),
			 extend_environment(procedure_parameters(procedure),
					    arguments,
					    procedure_environment(procedure)));
  } else
    return error("Unknown procedure type -- APPLY", 1, procedure);
}

int is_atom_char(char s)
{
  char *valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.!?+-*/=<>#";
  if (s && strchr(valid, s))
    return 1;
  else
    return 0;
}

int is_symbol(lisp *e)
{
  if (is_atom(e)) {
    int issym = 1;
    if (!is_atom_char(e->atom[0]))
      issym = 0;
    for (int i = 1; i < strlen(e->atom); i++)
      issym = issym && is_atom_char(e->atom[i]);
    return issym || is_number(e);
  } else {
    return 0;
  }
}

int is_err(lisp *e)
{
  if (!e)
    return 0;
  else if (is_null(e))
    return 0;
  else if (is_pair(e))
    return is_err(safe_car(e)) || is_err(safe_cdr(e));
  else
    return is_eq_str(e, "ERR");
}

int is_int(lisp *e)
{
  if (is_atom(e)) {
    int isint = e->atom[0] == '-' || isdigit(e->atom[0]);
    for (int i = 1; i < strlen(e->atom); i++)
      isint = isint && isdigit(e->atom[i]);
    return isint;
  } else {
    return 0;
  }
}

int is_double(lisp *e)
{
  if (is_atom(e)) {
    int isdouble = e->atom[0] == '-' || isdigit(e->atom[0]);
    int dots = 0;
    for (int i = 1; i < strlen(e->atom); i++)
      if (e->atom[i] == '.')
	dots++;
      else
	isdouble = isdouble && isdigit(e->atom[i]);
    return isdouble && dots < 2;
  } else {
    return 0;
  }
}

int is_boolean(lisp *e)
{
  return is_eq_str(e, "#t") || is_eq_str(e, "#f");
}

int is_number(lisp *e)
{
  return is_int(e) || is_double(e);
}

int is_string(lisp *e)
{
  if (is_atom(e)) {
    char first = e->atom[0];
    char last = e->atom[strlen(e->atom)-1];
    return (first == '"' && last == '"');
  } else {
    return 0;
  }
}

char *string_value(lisp *e)
{
  char *atom = e->atom;
  int len = strlen(atom) - 1;
  char *str = malloc(sizeof(char) * len);

  for (int i = 1; i < len; i++)
      str[i-1] = atom[i];
  str[len-1] = '\0';

  return str;
}

lisp *display(lisp *e)
{
  if (is_string(e)) {
    char *str = string_value(e);
    printf("%s", str);
    free(str);
  } else {
    prettyPrint(e);
    puts("");
  }

  return e;
}

int is_self_evaluating(lisp *exp)
{
  return is_boolean(exp) || is_number(exp) || is_string(exp) || is_primitive_procedure(exp);
}

int is_variable(lisp *exp)
{
  return is_symbol(exp);
}

int is_quoted(lisp *exp)
{
  return is_tagged_list(exp, "quote");
}

lisp *text_of_quotation(lisp *exp)
{
  return cadr(copy(exp));
}

int is_assignment(lisp *exp)
{
  return is_tagged_list(exp, "set!");
}

lisp *assignment_variable(lisp *exp)
{
  return cadr(exp);
}

lisp *assignment_value(lisp *exp)
{
  return caddr(exp);
}

int is_definition(lisp *exp)
{
  return is_tagged_list(exp, "define");
}

lisp *definition_variable(lisp *exp)
{
  if (is_symbol(cadr(exp)))
    return cadr(exp);
  else
    return caadr(exp);
}

lisp *make_lambda(lisp *parameters, lisp *body)
{
  return list(3, mkAtom("lambda"), parameters, body);
}

lisp *definition_value(lisp *exp)
{
  if (is_symbol(cadr(exp)))
    return caddr(exp);
  else
    return make_lambda(cdadr(exp),
		       caddr(exp));
}

lisp *set_variable_value_i(lisp *var, lisp *val, lisp *env)
{
  if (is_null(env)) {
    error("Unbound variable -- SET!", 1, var);
  } else {
    lisp *frame = first_frame(env);
    def_scan(var,
	     val,
	     frame_variables(frame),
	     frame_values(frame),
	     env);
    set_car_i(env, frame);
  }

  return mkAtom("ok");
}

void def_scan(lisp *var, lisp *val, lisp *vars, lisp *vals, lisp *frame)
{
  if (is_null(vars))
    add_binding_to_frame_i(var, val, frame);
  else if (is_eq(var, car(vars)))
    set_car_i(vals, val);
  else
    def_scan(var, val, cdr(vars), cdr(vals), frame);
}

lisp *eval_assignment(lisp *exp, lisp *env)
{
  set_variable_value_i(assignment_variable(exp),
		       eval(assignment_value(exp), env),
		       env);
  return mkAtom("ok");
}

void define_variable_i(lisp *var, lisp *val, lisp *env)
{
  lisp *frame = first_frame(env);
  def_scan(var,
	   val,
	   frame_variables(frame),
	   frame_values(frame),
	   frame);
  set_car_i(env, frame);
}

lisp *eval_definition(lisp *exp, lisp *env)
{
  define_variable_i(definition_variable(exp),
		    eval(definition_value(exp), env),
		    env);
  return mkAtom("ok");
}

int is_lambda(lisp *exp)
{
  return is_tagged_list(exp, "lambda") || is_tagged_list(exp, "λ");
}

lisp *lambda_parameters(lisp *exp)
{
  return cadr(exp);
}

lisp *lambda_body(lisp *exp)
{
  return cddr(exp);
}

lisp *make_begin(lisp *seq)
{
  return cons(mkAtom("begin"), seq);
}

lisp *sequence_exp(lisp *seq)
{
  if (is_null(seq))
    return seq;
  else if (is_last_exp(seq))
    return first_exp(seq);
  else
    return make_begin(seq);
}

int is_application(lisp *exp)
{
  return length(exp) >= 1;
  // return is_pair(exp);
}

lisp *operator(lisp *exp)
{
  return car(exp);
}

lisp *operands(lisp *exp)
{
  return cdr(exp);
}

int is_no_operands(lisp *ops)
{
  return is_null(ops);
}

lisp *first_operand(lisp *ops)
{
  return car(ops);
}

lisp *rest_operands(lisp *ops)
{
  return cdr(ops);
}

int is_cond(lisp *exp)
{
  return is_tagged_list(exp, "cond");
}

lisp *cond_clauses(lisp *exp)
{
  return cdr(exp);
}

lisp *cond_predicate(lisp *clause)
{
  return car(clause);
}

int is_cond_else_clause(lisp *clause)
{
  return is_eq_str(cond_predicate(clause), "else");
}

lisp *cond_actions(lisp *clause)
{
  return cdr(clause);
}

lisp *expand_clauses(lisp *clauses)
{
  if (is_null(clauses)) {
    return mkAtom("#f");
  } else {
    lisp *first = car(clauses);
    lisp *rest = cdr(clauses);
    if (is_cond_else_clause(first)) {
      if (is_null(rest))
	return sequence_exp(cond_actions(first));
      else
	return error("ELSE clause isn't last -- COND->IF", 1, clauses);
    } else {
      return make_if(cond_predicate(first),
		     sequence_exp(cond_actions(first)),
		     expand_clauses(rest));
    }
  }
}

lisp *cond_if(lisp *exp)
{
  return expand_clauses(cond_clauses(exp));
}

lisp *list_of_values(lisp *exps, lisp *env)
{
  if (is_no_operands(exps))
    return mkAtom("nil");
  else
    return cons(eval(first_operand(exps), env),
		list_of_values(rest_operands(exps),
			       env));
}

lisp *begin_actions(lisp *exp)
{
  return cdr(exp);
}

int is_begin(lisp *exp)
{
  return is_tagged_list(exp, "begin");
}

int is_let(lisp *exp)
{
  return is_tagged_list(exp, "let");
}

lisp *map_car(lisp *exps)
{
  if (is_null(exps))
    return mkAtom("nil");
  else
    return cons(car(car(exps)),
		map_car(cdr(exps)));
}

lisp *map_cadr(lisp *exps)
{
  if (is_null(exps))
    return mkAtom("nil");
  else
    return cons(cadr(car(exps)),
		map_cadr(cdr(exps)));
}

lisp *let_vars(lisp *exp)
{
  return map_car(cadr(exp));
}

lisp *let_vals(lisp *exp)
{
  return map_cadr(cadr(exp));
}

lisp *let_body(lisp *exp)
{
  return caddr(exp);
}

lisp *let_combination(lisp *exp)
{
  return cons(make_lambda(let_vars(exp),
			  let_body(exp)),
	      let_vals(exp));
}

int is_load(lisp *exp)
{
  return is_tagged_list(exp, "load");
}

lisp *file_to_load(lisp *exp)
{
  return cadr(exp);
}

lisp *load_file(lisp *file, lisp *env)
{
  char *fn_local = string_value(file);
  FILE *fp_local = fopen(fn_local, "r");

  char *share = "/usr/share/ulisp/";
  char *fn_share = malloc(sizeof(char) * (strlen(share) + strlen(fn_local) + 1));
  fn_share = strcpy(fn_share, share);
  fn_share = strcat(fn_share, fn_local);
  FILE *fp_share = fopen(fn_share, "r");

  FILE *fp = NULL;
  if (fp_local)
    fp = fp_local;
  else if (fp_share)
    fp = fp_share;
  else
    return error("Unable to load file", 1, file);

  int eof = 0;
  while (!eof) {
    char *buffer = calloc(1, sizeof(char));
    do {
      char line[128];
      if (fgets(line, 128, fp)) {
	buffer = realloc(buffer, strlen(buffer) + strlen(line) + 1);
	buffer = strcat(buffer, line);
      } else {
	eof = 1;
      }
    } while (parens(buffer) > 0);
    
    if (parens(buffer) == 0) {
      int i = 0;
      lisp *input = parse(buffer, &i);
      free(buffer);
      if (input) {
	eval(input, env);
	freeLisp(input);
      }
    } else {
      fputs("Unbalanced S-Expression!\n", stderr);
    }
  }

  fclose(fp);
  free(fn_local);
  free(fn_share);
  
  return mkAtom("ok");
}

lisp *eval(lisp *e, lisp *env)
{
  lisp *exp = copy(e);
  if (DEBUG) {
    printf("eval  : ");
    prettyPrint(exp);
    puts("");
  }
  if (is_err(exp))
    return mkAtom("ERR");
  else if (is_self_evaluating(exp))
    return mkAtom(exp->atom);
  else if (is_variable(exp))
    return lookup_variable_value(exp, env);
  else if (is_quoted(exp))
    return text_of_quotation(exp);
  else if (is_assignment(exp))
    return eval_assignment(exp, env);
  else if (is_definition(exp))
    return eval_definition(exp, env);
  else if (is_if(exp))
    return eval_if(exp, env);
  else if (is_lambda(exp))
    return make_procedure(lambda_parameters(exp),
			  lambda_body(exp),
			  env);
  else if (is_begin(exp))
    return eval_sequence(begin_actions(exp), env);
  else if (is_cond(exp))
    return eval(cond_if(exp), env);
  else if (is_let(exp))
    return eval(let_combination(exp), env);
  else if (is_load(exp)) {
    return load_file(file_to_load(exp), env);
  } else if (is_application(exp))
    return apply(eval(operator(exp), env),
		 list_of_values(operands(exp), env));
  else
    return error("Unknown expression type -- EVAL", 1, exp);
}

int length(lisp *l)
{
  if (is_null(l))
    return 0;
  else if (is_atom(l))
    return 1;
  else
    return 1 + length(cdr(l));
}

void showLisp (lisp *l)
{
  if (is_atom(l))
    printf("%s", l->atom);
  if (l->fst) {
    putchar('(');
    showLisp(car(l));
  }
  if (l->snd) {
    printf(" . ");
    showLisp(cdr(l));
    putchar(')');
  }
}

lisp *nth(lisp *l, int i)
{
  if (i == 0)
    return car(l);
  else
    return nth(cdr(l), --i);
}

void prettyPrint(lisp *l)
{
  if (l) {
    if (is_atom(l)) {
      printf("%s", l->atom);
    } else if (is_compound_procedure(l)) {
      printf("λ");
    } else {
      putchar('(');
      int len = length(l);
      for (int i = 0; i < len; i++) {
	prettyPrint(nth(l, i));
	if (i+1 < len)
	  putchar(' ');
      }
      putchar(')');
    }
  }
}

void s(lisp *l)
{
  prettyPrint(l);
  puts("");
}

lisp *parseAtom(char *s, int *i)
{
  char *astr = calloc(sizeof(char), 1);
  int len = 0;

  while (is_atom_char(s[*i])) {
    astr = realloc(astr, sizeof(char) * len+2);
    astr[len] = s[*i];
    astr[len+1] = '\0';
    ++len;
    ++*i;
  }

  lisp *a = mkAtom(astr);
  free(astr);

  return a;
}

lisp *parseString(char *s, int *i)
{
  char *astr = calloc(sizeof(char), 1);
  int len = 0;
  int escaped = 0;

  astr = realloc(astr, sizeof(char) * len+2);
  astr[len] = '"';
  astr[len+1] = '\0';
  ++len;
  ++*i;

  while (1) {
    if (!escaped && s[*i] == '"')
      break;
    escaped = 0;

    if (s[*i] == '\\')
      escaped = 1;

    astr = realloc(astr, sizeof(char) * len+2);
    if (escaped && s[*i] == 'n') {
      astr[len] = '\n';
      escaped = 0;
    } else {
      astr[len] = s[*i];
    }
    astr[len+1] = '\0';
    ++len;
    ++*i;
  }

  astr = realloc(astr, sizeof(char) * len+2);
  astr[len] = '"';
  astr[len+1] = '\0';
  ++len;
  ++*i;

  lisp *a = mkAtom(astr);
  free(astr);

  return a;
}

int parens(char *s)
{
  int depth = 0;
  for (int i = 0; i < strlen(s); i++)
    if (s[i] == '(')
      depth++;
    else if (s[i] == ')')
      depth--;
  return depth;
}

/* Parse an S-Expression */
lisp *parse(char *s, int *i)
{
  if (*i < strlen(s)) {
    if (is_atom_char(s[*i])) {
      lisp *a = parseAtom(s, i);
      lisp *b = parse(s, i);
      return cons(a, b);
    } else if (s[*i] == '"') {
      lisp *a = parseString(s, i);
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
    } else if (s[*i] == ' ' || s[*i] == '\n') {
      ++*i;
      return parse(s, i);
    } else if (s[*i] == '\'') {
      ++*i;
      lisp *a = parse(s, i);
      // ++*i;
      // lisp *b = parse(s, i);
      return cons(mkAtom("quote"), cons(a, mkAtom("nil")));
      // return list(2, mkAtom("quote"), a);
    } else if (s[*i] == ';') {
      while (s[*i] != '\n' && *i < strlen(s))
	++*i;
      return parse(s, i);
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }
}

lisp *random(lisp *modulus)
{
  int m = floor(ltof(modulus));

  return ftol(rand() % m);
}
