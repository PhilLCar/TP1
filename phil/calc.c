#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Digits {
  int            digit;
  struct Digits *next;
} digits;

typedef struct {
  int     flags; // compteur de référence : flags >> 1
                 // flag de négativité    : flags &  1
  digits *value;
} bigint;

struct Stack {
  bigint **ptr;
  int      len;
  int      cap;
} stack = { NULL, 0, 0 };

void parsenumber(int c);
void parseoperator(int c);
void parsevariable();
void checkstack();
void add();
void sub();
void mul();
void print();

void alert(char* message)
{
  printf(message);
  exit(1);
}

void freeb(bigint *b) {
  digits *next, *val = b->value;
  free(b);
  while (val != NULL) {
    next = val->next;
    free(val);
    val = next;
  }
}

bigint **stackat(int index) {
  return (stack.ptr + stack.len - 1 - index);
}

void shrink() {
  // Dans le cas de ANS
  if (stack.len < 2) {
    printf("(ANS) ");
    return;
  }
  // Ne devrait pas arriver
  if (stack.len > 2) alert("WTF!!");
  // Cas normal
  freeb(*stack.ptr);
  *stack.ptr = *(stack.ptr + 1);
  stack.len = 1;
}

void push(bigint *elem)
{
  bigint **tmp;
  stack.len++;
  if (stack.len > stack.cap) {
    stack.cap = stack.len;
    tmp = realloc(stack.ptr, stack.cap * sizeof(bigint*));
    if (tmp == NULL) alert("STACK FAIL");
    stack.ptr = tmp;
  }
  *stackat(0) = elem;
}

bigint *pop()
{
  bigint  *ret = *stackat(0);
  stack.len--;
  if (stack.len > 0) {
    bigint **tmp = realloc(stack.ptr, stack.len * sizeof(bigint*));
    if (tmp != NULL) {
      stack.cap--;
      stack.ptr = tmp;
    }
  }
  return ret;
}

void swap()
{
  bigint *tmp;
  tmp = *stackat(0);
  *stackat(0) = *stackat(1);
  *stackat(1) = tmp;
}

void add()
{
  int ret;
  if ((ret = (*stackat(0))->flags & 1) != (*stackat(1))->flags & 1) {
    if (!ret) {
      (*stackat(1))->flags &= ~1;
      swap();
    }
    sub();
    return;
  }
  bigint *n1 = pop();
  bigint *n2 = *stackat(0);
  digits *v1 = n1->value;
  digits *v2 = n2->value;
  
  bigint *r  = malloc(sizeof(bigint));
  digits *res = malloc(sizeof(digits));
  
  int sum;

  r->value = res;
  r->flags = n1->flags & n2->flags & 1;
  ret = 0;
  
  goto adding;
  
  while (v1 != NULL && v2 != NULL) {   
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
  adding:
    res->digit = (sum = v1->digit + v2->digit + ret) % 10;
    ret = sum / 10;
    v1 = v1->next;
    v2 = v2->next;
  }
  while (v1 != NULL) {
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
    res->digit = (sum = v1->digit + ret) % 10;
    ret = sum / 10;
    v1 = v1->next;
  }
  while (v2 != NULL) {
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
    res->digit = (sum = v2->digit + ret) % 10;
    ret = sum / 10;
    v2 = v2->next;
  }
  if (ret) {
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL");
    else res = res->next;
    res->digit = 1;
  }
  res->next = NULL;
  
  freeb(n1);
  freeb(n2);
  
  *stackat(0) = r;
}

void sub()
{
  if ((*stackat(0))->flags & 1) {
    if ((*stackat(1))->flags & 1) {
      (*stackat(0))->flags &= ~1;
      (*stackat(1))->flags &= ~1;
      swap();
    } else {
	(*stackat(0))->flags |= 1;
	add();
	return;
    }
  } else if ((*stackat(1))->flags & 1) {
    (*stackat(0))->flags |= 1;
      add();
      return;
  }
  bigint *n1 = pop();
  bigint *n2 = *stackat(0);
  digits *v1 = n1->value;
  digits *v2 = n2->value;
  
  bigint *r  = malloc(sizeof(bigint));
  digits *res = malloc(sizeof(digits));
  digits *zero = NULL;
  
  int ret = 0, diff;
  
  r->value = res;
  
  goto substract;
  
  while (v1 != NULL && v2 != NULL) {   
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
  substract:
    if ((diff = v2->digit - v1->digit - ret) < 0) {
      res->digit = diff + 10;
      ret = 1;
    } else {
      res->digit = diff;
      ret = 0;
    }
    if (res->digit != 0) zero = res;
    v1 = v1->next;
    v2 = v2->next;
  }
  while (v1 != NULL) {
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
    if ((diff = -v1->digit - ret) < 0) {
      res->digit = diff + 10;
      ret = 1;
    } else {
      res->digit = diff;
      ret = 0;
    }
    if (res->digit != 0) zero = res;
    v1 = v1->next;
  }
  while (v2 != NULL) {
    res->next = malloc(sizeof(digits));
    if (res->next == NULL)
      alert("FAIL!");
    else res = res->next;
    if ((diff = v2->digit - ret) < 0) {
      res->digit = diff + 10;
      ret = 1;
    } else {
      res->digit = diff;
      ret = 0;
    }
    if (res->digit != 0) zero = res;
    v2 = v2->next;
  }
  res->next = NULL;

  // libère la mémoire occupée par les opérandes
  freeb(n1);
  freeb(n2);

  // si le nombre est négatif, inverse les digits
  if (r->flags = ret) {
    res = r->value;
    res->digit = 10 - res->digit;
    if (res->digit != 0) zero = res;
    res = res->next;
    while (res != NULL) {
      res->digit = 10 - res->digit - ret;
      if (res->digit != 0) zero = res;
      res = res->next;
    }
  }

  // si le nombre diminue de taille, enlève les zéros en trop;
  if (zero->next != NULL) {
    res = zero->next;
    zero->next = NULL;
    while (res->next != NULL) {
      zero = res->next;
      free(res);
      res = zero;
    }
  }
  
  *stackat(0) = r;
}

void mul()
{
  bigint *n1 = pop();
  bigint *n2 = pop();
  digits *v1 = n1->value;
  digits *v2;
  
  bigint *r;
  digits *res;
  
  int ret, prod, lvl = 0;
  int i;
  
  while (v1 != NULL) {
    v2 = n2->value;
    r = malloc(sizeof(bigint));
    res = malloc(sizeof(digits));
    r->value = res;
    r->flags = 0;
    ret = 0;
    for (i = 0; i < lvl; i++) {
      res->digit = 0;
      res->next = malloc(sizeof(digits));
      res = res->next;
    }
    goto multiply;
    while (v2 != NULL) {
      res->next = malloc(sizeof(digits));
      if (res->next == NULL)
	alert("FAIL!");
      else res = res->next;
    multiply:
      res->digit = (prod = v1->digit * v2->digit + ret) % 10;
      ret = prod / 10;
      v2 = v2->next;
    }
    if (ret) {
      res->next = malloc(sizeof(digits));
      res = res->next;
      res->digit = ret;
    }
    res->next = NULL;
    push(r);
    lvl++;
    v1 = v1->next;
  }
  while (--lvl > 0) {
    add();
  }
  r->flags = (n1->flags ^ n2->flags) & 1;

  freeb(n1);
  freeb(n2);
}

void print(bigint b)
{
  digits *i = b.value;
  digits *a = NULL, *p;
  if (b.flags & 1) printf("-");
  do {
    p = i->next;
    i->next = a;
    a = i;
    i = p;
  } while (i != NULL);
  i = a;
  a = NULL;
  do {
    printf("%d", i->digit);
    p = i->next;
    i->next = a;
    a = i;
    i = p;
  } while (i != NULL);
}

void parse()
{
  int c;
  while ((c = getchar()) != '\n') {
    if (c >= '0' && c <= '9')
      parsenumber(c);
    else if (c == '*' || c == '+' || c == '-' || c == '/' || c == '=')
      parseoperator(c);
    else if (c == EOF) {
      printf("exit\n");
      exit(1);
    }
    else if (c == ' ' || c == '\t')
      continue;
    else
      parsevariable();
  }
}

void parsenumber(int c)
{
  bigint *big = malloc(sizeof(bigint));
  digits *dig = malloc(sizeof(digits)), *tmp;
  if (big == NULL || dig == NULL)
    alert("Memory error!");
  big->flags = 0;
  dig->digit = c & 0x0F;
  dig->next  = NULL;
  while ((c = getchar()) >= '0' && c <= '9') {
    tmp = dig;
    dig = malloc(sizeof(digits));
    if (dig == NULL)
      alert("ULTIMATE FAIL");
    dig->digit = c & 0x0F;
    dig->next = tmp;
  }
  big->value = dig;
  ungetc(c, stdin);
  push(big);
}

void parseoperator(int c)
{
  switch(c) {
  case '+':
    add();
    break;
  case '-':
    sub();
    break;
  case '*':
    mul();
    break;
  case '/': // peut-être un jour
    //div();
    break;
  case '=':
    exit(1);
    //newvar();
  }
}

void parsevariable()
{
  // peut-être ajouter un parse exit
}

int main()
{
  // Initialisation
  stack.ptr = malloc(sizeof(bigint*));
  if (stack.ptr != NULL) stack.cap = 1;
  bigint *b = malloc(sizeof(bigint*));
  digits *d = malloc(sizeof(digits*));
  b->flags = 0;
  b->value = d;
  d->digit = 0;
  d->next = NULL;
  push(b);
  // Calculatrice
  while(1) {
    printf("> ");
    parse();
    shrink();
    print(**stackat(0));
    printf("\n");
  }
  return 0;
}
