#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "polynomial.h"

/* Static prototypes */
static int term_count(const polynomial *eqn);
static polynomial *_poly_op(const polynomial *a, const polynomial *b, int op);
static void _poly_rm_end(polynomial *p);

polynomial *term_create(int coeff, unsigned int exp)
{
    polynomial *head = malloc(sizeof(polynomial));

    if (head) {
        head->coeff = coeff;
        head->exp   = exp;
        head->next  = NULL;
    }

    return head;
}

void poly_destroy(polynomial *list)
{
    while (list) {
        polynomial *tmp = list->next;
        free(list);
        list = tmp;
    }
}

void poly_print(const polynomial *list)
{
    if (list == NULL) {
        printf("List empty\n");
        return;
    }

    if (list->coeff) {
        printf("%c%d", list->coeff > 0 ? '+' : '\0', list->coeff);
        if (list->exp > 1)
            printf("x^%d", list->exp);
        else if (list->exp == 1)
            printf("x");
        else if (list->exp == 0) {
            putchar('\n');
            return;
        }

        printf(" ");
    }

    if (list->next)
        poly_print(list->next);
    else {
        putchar('\n');
        return;
    }
}

char *poly_to_string(const polynomial *p)
{
    if (!p) {
        printf("List empty\n");
        return NULL;
    }

    polynomial *tmp = (polynomial *) p;
    int len = term_count(tmp);
    char *s = (char *) "";

    for (int i = 0; i < len; i++) {
        if (tmp->coeff < 0)
            asprintf(&s, "%s%d", s, tmp->coeff);
        else
            asprintf(&s, "%s%c%d", s, '+', tmp->coeff);

        if (tmp->exp > 1)
            asprintf(&s, "%sx^%d ", s, tmp->exp);
        else if (tmp->exp == 1)
            asprintf(&s, "%sx ", s);
        tmp = tmp->next;
    }
    return s;
}

polynomial *poly_add(const polynomial *a, const polynomial *b)
{
    return _poly_op(a, b, ADDITION);
}

void poly_iterate(polynomial *p, void (*transform)(struct term *))
{
    while (p) {
        (*transform)(p);
        p = p->next;
    }
}

double poly_eval(const polynomial *a, double x)
{
    double ret = 0;
    polynomial *tmp = (polynomial *) a;

    while (tmp) {
        ret += pow(x, tmp->exp) * tmp->coeff;
        tmp = tmp->next;
    }

    return ret;
}

bool poly_equal(const polynomial *a, const polynomial *b)
{
    char *a_str = poly_to_string(a);
    char *b_str = poly_to_string(b);

    int ret = strcmp(a_str, b_str);
    free(a_str);
    free(b_str);

    return ret ? false : true;
}

polynomial *poly_sub(const polynomial *a, const polynomial *b)
{
    return _poly_op(a, b, SUBTRACTION);
}

/* Static functions */

static int term_count(const polynomial *eqn)
{
    /* Returns the number of terms in eqn */
    int c = 0;
    polynomial *tmp = (polynomial *) eqn;

    while (tmp) {
        c++;
        tmp = tmp->next;
    }

    return c;
}

static polynomial *_poly_op(const polynomial *a, const polynomial *b, int op)
{
    /* Performs the operation op on a and b */
    polynomial *a_head = (polynomial *) a;
    polynomial *b_head = (polynomial *) b;
    polynomial *ret = term_create(0,0U);
    polynomial *head = ret;

    while (a_head->next && b_head->next) {

        if (a_head->exp > b_head->exp) { /* If a is greater than b */
            ret->coeff = (op == ADDITION) ? (a_head->coeff) : -(a_head->coeff);
            ret->exp = a_head->exp;
            a_head = a_head->next;
        } else if (b_head->exp > a_head->exp) { /* If b is greater than a */
            ret->coeff = (op == ADDITION) ? (b_head->coeff) : -(b_head->coeff);
            ret->exp = b_head->exp;
            b_head = b_head->next;
        } else { /* Exponents are the same */
            ret->exp = a_head->exp;
            ret->coeff = (op == ADDITION) ? (a_head->coeff + b_head->coeff) : (a_head->coeff - b_head->coeff);
            a_head = a_head->next;
            b_head = b_head->next;
        }

        ret->next = term_create(0, 0U);
        ret = ret->next;
    }

    if (a_head->coeff && b_head->coeff) {
            ret->exp = a_head->exp;
            ret->coeff = (op == ADDITION) ? (a_head->coeff + b_head->coeff) : (a_head->coeff - b_head->coeff);
            a_head = a_head->next;
            b_head = b_head->next;

            if (!(ret->coeff) && !(ret->exp))
                _poly_rm_end(head);

            return head;
    }

    while (a_head->next || b_head->next) {
        if (a_head->next) {
            ret->coeff = a_head->coeff;
            ret->exp = a_head->exp;
            a_head = a_head->next;
        }
        if (b_head->exp > a_head->exp) {
            ret->coeff = b_head->coeff;
            ret->exp = b_head->exp;
            b_head = b_head->next;
        }

        ret->next = term_create(0, 0U);
        ret = ret->next;
    }

    /* Remove unneeded node from end of list */
    if (!(ret->coeff) && !(ret->exp))
        _poly_rm_end(head);

    return head;
}

static void _poly_rm_end(polynomial *p)
{
    /* Removes last term from p */
    polynomial *tmp = p, *t;

    while (tmp->next) {
        t = tmp;
        tmp = tmp->next;
    }
    free(t->next);
    t->next = NULL;
}
