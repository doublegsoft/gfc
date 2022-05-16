/*
** gfc
**
** Copyright (C) 2020 doublegsoft.open
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gfc_sql.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <json-c/json.h>

#include "gfc_map.h"

struct mustach_itf {
  int (*start)(void *closure);
  int (*put)(void *closure, const char *name, int escape, FILE *file);
  int (*enter)(void *closure, const char *name);
  int (*next)(void *closure);
  int (*leave)(void *closure);
};

#define MUSTACH_OK                       0
#define MUSTACH_ERROR_SYSTEM            -1
#define MUSTACH_ERROR_UNEXPECTED_END    -2
#define MUSTACH_ERROR_EMPTY_TAG         -3
#define MUSTACH_ERROR_TAG_TOO_LONG      -4
#define MUSTACH_ERROR_BAD_SEPARATORS    -5
#define MUSTACH_ERROR_TOO_DEPTH         -6
#define MUSTACH_ERROR_CLOSING           -7
#define MUSTACH_ERROR_BAD_UNESCAPE_TAG  -8

#define NAME_LENGTH_MAX   1024
#define DEPTH_MAX         256

#if !defined(NO_OPEN_MEMSTREAM)
static FILE *memfile_open(char **buffer, size_t *size)
{
  return open_memstream(buffer, size);
}
static void memfile_abort(FILE *file, char **buffer, size_t *size)
{
  fclose(file);
  free(*buffer);
  *buffer = NULL;
  *size = 0;
}
static int memfile_close(FILE *file, char **buffer, size_t *size)
{
  int rc;

  /* adds terminating null */
  rc = fputc(0, file) ? MUSTACH_ERROR_SYSTEM : 0;
  fclose(file);
  if (rc == 0)
    /* removes terminating null of the length */
    (*size)--;
  else {
    free(*buffer);
    *buffer = NULL;
    *size = 0;
  }
  return rc;
}
#else
static FILE *memfile_open(char **buffer, size_t *size)
{
  return tmpfile();
}
static void memfile_abort(FILE *file, char **buffer, size_t *size)
{
  fclose(file);
  *buffer = NULL;
  *size = 0;
}
static int memfile_close(FILE *file, char **buffer, size_t *size)
{
  int rc;
  size_t s;
  char *b;

  s = (size_t)ftell(file);
  b = malloc(s + 1);
  if (b == NULL) {
    rc = MUSTACH_ERROR_SYSTEM;
    errno = ENOMEM;
    s = 0;
  } else {
    rewind(file);
    if (1 == fread(b, s, 1, file)) {
      rc = 0;
      b[s] = 0;
    } else {
      rc = MUSTACH_ERROR_SYSTEM;
      free(b);
      b = NULL;
      s = 0;
    }
  }
  *buffer = b;
  *size = s;
  return rc;
}
#endif

static int getpartial(struct mustach_itf *itf, void *closure, const char *name, char **result)
{
  int rc;
  FILE *file;
  size_t size;

  *result = NULL;
  file = memfile_open(result, &size);
  if (file == NULL)
    rc = MUSTACH_ERROR_SYSTEM;
  else {
    rc = itf->put(closure, name, 0, file);
    if (rc < 0)
      memfile_abort(file, result, &size);
    else
      rc = memfile_close(file, result, &size);
  }
  return rc;
}

static int process(const char *template, struct mustach_itf *itf, void *closure, FILE *file, const char *opstr, const char *clstr)
{
  char name[NAME_LENGTH_MAX + 1], *partial, c, *tmp;
  const char *beg, *term;
  struct { const char *name, *again; size_t length; int emit, entered; } stack[DEPTH_MAX];
  size_t oplen, cllen, len, l;
  int depth, rc, emit;

  emit = 1;
  oplen = strlen(opstr);
  cllen = strlen(clstr);
  depth = 0;
  for(;;) {
    beg = strstr(template, opstr);
    if (beg == NULL) {
      /* no more mustach */
      if (emit)
        fwrite(template, strlen(template), 1, file);
      return depth ? MUSTACH_ERROR_UNEXPECTED_END : 0;
    }
    if (emit)
      fwrite(template, (size_t)(beg - template), 1, file);
    beg += oplen;
    term = strstr(beg, clstr);
    if (term == NULL)
      return MUSTACH_ERROR_UNEXPECTED_END;
    template = term + cllen;
    len = (size_t)(term - beg);
    c = *beg;
    switch(c) {
    case '!':
    case '=':
      break;
    case '{':
      for (l = 0 ; clstr[l] == '}' ; l++);
      if (clstr[l]) {
        if (!len || beg[len-1] != '}')
          return MUSTACH_ERROR_BAD_UNESCAPE_TAG;
        len--;
      } else {
        if (term[l] != '}')
          return MUSTACH_ERROR_BAD_UNESCAPE_TAG;
        template++;
      }
      c = '&';
      /*@fallthrough@*/
    case '^':
    case '#':
    case '/':
    case '&':
    case '>':
#if !defined(NO_EXTENSION_FOR_MUSTACH) && !defined(NO_COLON_EXTENSION_FOR_MUSTACH)
    case ':':
#endif
      beg++; len--;
    default:
      while (len && isspace(beg[0])) { beg++; len--; }
      while (len && isspace(beg[len-1])) len--;
#if defined(NO_EXTENSION_FOR_MUSTACH) || defined(NO_ALLOW_EMPTY_TAG)
      if (len == 0)
        return MUSTACH_ERROR_EMPTY_TAG;
#endif
      if (len > NAME_LENGTH_MAX)
        return MUSTACH_ERROR_TAG_TOO_LONG;
      memcpy(name, beg, len);
      name[len] = 0;
      break;
    }
    switch(c) {
    case '!':
      /* comment */
      /* nothing to do */
      break;
    case '=':
      /* defines separators */
      if (len < 5 || beg[len - 1] != '=')
        return MUSTACH_ERROR_BAD_SEPARATORS;
      beg++;
      len -= 2;
      for (l = 0; l < len && !isspace(beg[l]) ; l++);
      if (l == len)
        return MUSTACH_ERROR_BAD_SEPARATORS;
      oplen = l;
#ifdef _WIN32
      tmp = _alloca(oplen + 1);
#else
      tmp = alloca(oplen + 1);
#endif
      memcpy(tmp, beg, oplen);
      tmp[oplen] = 0;
      opstr = tmp;
      while (l < len && isspace(beg[l])) l++;
      if (l == len)
        return MUSTACH_ERROR_BAD_SEPARATORS;
      cllen = len - l;
#ifdef _WIN32
      tmp = _alloca(cllen + 1);
#else
      tmp = alloca(cllen + 1);
#endif
      memcpy(tmp, beg + l, cllen);
      tmp[cllen] = 0;
      clstr = tmp;
      break;
    case '^':
    case '#':
      /* begin section */
      if (depth == DEPTH_MAX)
        return MUSTACH_ERROR_TOO_DEPTH;
      rc = emit;
      if (rc) {
        rc = itf->enter(closure, name);
        if (rc < 0)
          return rc;
      }
      stack[depth].name = beg;
      stack[depth].again = template;
      stack[depth].length = len;
      stack[depth].emit = emit;
      stack[depth].entered = rc;
      if ((c == '#') == (rc == 0))
        emit = 0;
      depth++;
      break;
    case '/':
      /* end section */
      if (depth-- == 0 || len != stack[depth].length || memcmp(stack[depth].name, name, len))
        return MUSTACH_ERROR_CLOSING;
      rc = emit && stack[depth].entered ? itf->next(closure) : 0;
      if (rc < 0)
        return rc;
      if (rc) {
        template = stack[depth++].again;
      } else {
        emit = stack[depth].emit;
        if (emit && stack[depth].entered)
          itf->leave(closure);
      }
      break;
    case '>':
      /* partials */
      if (emit) {
        rc = getpartial(itf, closure, name, &partial);
        if (rc == 0) {
          rc = process(partial, itf, closure, file, opstr, clstr);
          free(partial);
        }
        if (rc < 0)
          return rc;
      }
      break;
    default:
      /* replacement */
      if (emit) {
        rc = itf->put(closure, name, c != '&', file);
        if (rc < 0)
          return rc;
      }
      break;
    }
  }
}

int fmustach(const char *template, struct mustach_itf *itf, void *closure, FILE *file)
{
  int rc = itf->start ? itf->start(closure) : 0;
  if (rc == 0)
    rc = process(template, itf, closure, file, "{{", "}}");
  return rc;
}

int fdmustach(const char *template, struct mustach_itf *itf, void *closure, int fd)
{
  int rc;
  FILE *file;

  file = fdopen(fd, "w");
  if (file == NULL) {
    rc = MUSTACH_ERROR_SYSTEM;
    errno = ENOMEM;
  } else {
    rc = fmustach(template, itf, closure, file);
    fclose(file);
  }
  return rc;
}

int mustach(const char *template, struct mustach_itf *itf, void *closure, char **result, size_t *size)
{
  int rc;
  FILE *file;
  size_t s;

  *result = NULL;
  if (size == NULL)
    size = &s;
  file = memfile_open(result, size);
  if (file == NULL)
    rc = MUSTACH_ERROR_SYSTEM;
  else {
    rc = fmustach(template, itf, closure, file);
    if (rc < 0)
      memfile_abort(file, result, size);
    else
      rc = memfile_close(file, result, size);
  }
  return rc;
}


#define MAX_DEPTH 256

#define SQL_NULL                  "null"
#define SQL_OPERATOR_LIKE         "like"

#if defined(NO_EXTENSION_FOR_MUSTACH)
# if !defined(NO_COLON_EXTENSION_FOR_MUSTACH)
#  define NO_COLON_EXTENSION_FOR_MUSTACH
# endif
# if !defined(NO_EQUAL_VALUE_EXTENSION_FOR_MUSTACH)
#  define NO_EQUAL_VALUE_EXTENSION_FOR_MUSTACH
# endif
# if !defined(NO_JSON_POINTER_EXTENSION_FOR_MUSTACH)
#  define NO_JSON_POINTER_EXTENSION_FOR_MUSTACH
# endif
#endif

#if defined(NO_COLON_EXTENSION_FOR_MUSTACH)
# if !defined(NO_JSON_POINTER_EXTENSION_FOR_MUSTACH)
#  define NO_JSON_POINTER_EXTENSION_FOR_MUSTACH
# endif
#endif

struct expl {
  struct json_object *root;
  int depth;
  struct {
    struct json_object *cont;
    struct json_object *obj;
    int index, count;
  } stack[MAX_DEPTH];
};

#if !defined(NO_EQUAL_VALUE_EXTENSION_FOR_MUSTACH)
static char *keyval(char *head, int isptr)
{
  char *w, c;

  c = *(w = head);
  while (c && c != '=') {
#if !defined(NO_JSON_POINTER_EXTENSION_FOR_MUSTACH)
    if (isptr) {
      if (isptr && c == '~' && head[1] == '=')
        c = *++head;
    } else
#endif
    if (!isptr && c == '\\' && head[1] == '=')
      c = *++head;
    *w++ = c;
    c = *++head;
  }
  *w = 0;
  return c == '=' ? ++head : NULL;
}
#endif

static char *key(char **head, int isptr)
{
  char *r, *i, *w, c;

  c = *(i = *head);
  if (!c)
    r = NULL;
  else {
    r = w = i;
#if !defined(NO_JSON_POINTER_EXTENSION_FOR_MUSTACH)
    if (isptr)
      while (c && c != '/') {
        if (c == '~')
          switch (i[1]) {
          case '1': c = '/'; /*@fallthrough@*/
          case '0': i++;
          }
        *w++ = c;
        c = *++i;
      }
    else
#endif
    while (c && c != '.') {
      if (c == '\\' && (i[1] == '.' || i[1] == '\\'))
        c = *++i;
      *w++ = c;
      c = *++i;
    }
    *w = 0;
    *head = i + !!c;
  }
  return r;
}

static struct json_object *find(struct expl *e, const char *name)
{
  int i, isptr;
  struct json_object *o;
  char *n, *c, *v;
#ifdef _WIN32
  n = _alloca(1 + strlen(name));
#else
  n = alloca(1 + strlen(name));
#endif
  strcpy(n, name);
  isptr = 0;
#if !defined(NO_JSON_POINTER_EXTENSION_FOR_MUSTACH)
  isptr = n[0] == '/';
  n += isptr;
#endif

  v = NULL;
#if !defined(NO_EQUAL_VALUE_EXTENSION_FOR_MUSTACH)
  v = keyval(n, isptr);
#endif
  if (n[0] == '.' && !n[1]) {
    /* case of . alone */
    o = e->stack[e->depth].obj;
  } else {
    c = key(&n, isptr);
    if (c == NULL)
      return NULL;
    o = NULL;
    i = e->depth;
    while (i >= 0 && !json_object_object_get_ex(e->stack[i].obj, c, &o))
      i--;
    if (i < 0)
      return NULL;
    c = key(&n, isptr);
    while(c) {
      if (!json_object_object_get_ex(o, c, &o))
        return NULL;
      c = key(&n, isptr);
    }
  }
  if (v) {
    i = v[0] == '!';
    if (i == !strcmp(&v[i], json_object_get_string(o)))
      o = NULL;
  }
  return o;
}

static int start(void *closure)
{
  struct expl *e = closure;
  e->depth = 0;
  e->stack[0].cont = NULL;
  e->stack[0].obj = e->root;
  e->stack[0].index = 0;
  e->stack[0].count = 1;
  return 0;
}

static void print(FILE *file, const char *string, int escape)
{
  if (!escape)
    fprintf(file, "%s", string);
  else if (*string)
    do {
      switch(*string) {
      case '<': fputs("&lt;", file); break;
      case '>': fputs("&gt;", file); break;
      case '&': fputs("&amp;", file); break;
      default: putc(*string, file); break;
      }
    } while(*++string);
}

static int put(void *closure, const char *name, int escape, FILE *file)
{
  struct expl *e = closure;
  char* operator = NULL;
  char  varname[64] = {'\0'};
  if (strstr(name, "_like") == name)
  {
    strcpy(varname, name + 6);
    varname[strlen(varname) - 1] = '\0';
    operator = SQL_OPERATOR_LIKE;
  }
  else
    strcpy(varname, name);
  struct json_object *o = find(e, varname);
  if (o)
  {
    // ADDED BY CHRISTIAN GANN
    // FOR SQL, WE SHOULD USE SINGLE-QUOTES.
    const char* str = json_object_get_string(o);
    if (str[0] == '\0')
      print(file, SQL_NULL, escape);
    else if (operator == SQL_OPERATOR_LIKE)
    {
      fputs("'%", file);
      print(file, str, escape);
      fputs("%'", file);
    }
    else
    {
      fputs("'", file);
      print(file, str, escape);
      fputs("'", file);
    }

  }
  else
  {
    // ADDED BY CHRISTIAN GANN
    // IF NOT FOUND, THE SQL NULL IS DEFAULT
    if (strcmp(varname, "lmt") == 0)
      print(file, "current_timestamp", escape);
    else
    {
      print(file, SQL_NULL, escape);
    }
  }
  return 0;
}

static int enter(void *closure, const char *name)
{
  struct expl *e = closure;
  struct json_object *o = find(e, name);
  if (++e->depth >= MAX_DEPTH)
    return MUSTACH_ERROR_TOO_DEPTH;
  if (json_object_is_type(o, json_type_array)) {
    e->stack[e->depth].count = json_object_array_length(o);
    if (e->stack[e->depth].count == 0) {
      e->depth--;
      return 0;
    }
    e->stack[e->depth].cont = o;
    e->stack[e->depth].obj = json_object_array_get_idx(o, 0);
    e->stack[e->depth].index = 0;
  } else if (json_object_is_type(o, json_type_object) || json_object_get_boolean(o)) {
    e->stack[e->depth].count = 1;
    e->stack[e->depth].cont = NULL;
    e->stack[e->depth].obj = o;
    e->stack[e->depth].index = 0;
  } else {
    e->depth--;
    return 0;
  }
  return 1;
}

static int next(void *closure)
{
  struct expl *e = closure;
  if (e->depth <= 0)
    return MUSTACH_ERROR_CLOSING;
  e->stack[e->depth].index++;
  if (e->stack[e->depth].index >= e->stack[e->depth].count)
    return 0;
  e->stack[e->depth].obj = json_object_array_get_idx(e->stack[e->depth].cont, e->stack[e->depth].index);
  return 1;
}

static int leave(void *closure)
{
  struct expl *e = closure;
  if (e->depth <= 0)
    return MUSTACH_ERROR_CLOSING;
  e->depth--;
  return 0;
}

static struct mustach_itf itf = {
  .start = start,
  .put = put,
  .enter = enter,
  .next = next,
  .leave = leave
};

int fmustach_json_c(const char *template, struct json_object *root, FILE *file)
{
  struct expl e;
  e.root = root;
  return fmustach(template, &itf, &e, file);
}

int fdmustach_json_c(const char *template, struct json_object *root, int fd)
{
  struct expl e;
  e.root = root;
  return fdmustach(template, &itf, &e, fd);
}

int mustach_json_c(const char *template, struct json_object *root, char **result, size_t *size)
{
  struct expl e;
  e.root = root;
  return mustach(template, &itf, &e, result, size);
}

static gfc_map_p _gfc_sqls = NULL;

GFC_API int
gfc_sql_set(const char* id, const char* sql)
{
  if (_gfc_sqls == NULL) _gfc_sqls = gfc_map_new();

  gfc_map_put(_gfc_sqls, id, (user_data)sql);

  return GFC_ERROR_SQL_OK;
}

GFC_API int
gfc_sql_get(const char* id, const char* json, char** sql)
{
  if (_gfc_sqls == NULL)
    return GFC_ERROR_SQL_NOT_FOUND;

  char* sqltpl = NULL;

  if (gfc_map_get(_gfc_sqls, id, (user_data*)&sqltpl) == GFC_ERROR_MAP_MISSING)
    return GFC_ERROR_SQL_NOT_FOUND;

  size_t size = 0;

  json_object* params = json_tokener_parse(json);
  int rc = mustach_json_c(sqltpl, params, sql, &size);

  return GFC_ERROR_SQL_OK;
}
