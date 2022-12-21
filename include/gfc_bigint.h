/*
**           .d888
**          d88P"
**          888
**  .d88b.  888888 .d8888b
** d88P"88b 888   d88P"
** 888  888 888   888
** Y88b 888 888   Y88b.
**  "Y88888 888    "Y8888P
**      888
** Y8b d88P
**  "Y88P"
*/

#ifndef __GFC_MATH_H__
#define __GFC_MATH_H__

#ifdef  __cplusplus
extern "C" {
#endif

extern typedef struct gfc_bigint_s;

typedef struct gfc_bigint_s   gfc_bigint_t;
typedef        gfc_bigint_t*  gfc_bigint_p;

gfc_bigint_p
gfc_bigint_new(const char* val);

void
gfc_bigint_free(gfc_bigint_p bigint);

gfc_bigint_p
gfc_bigint_add(gfc_bigint_p augend, gfc_bigint_p addend);

gfc_bigint_p
gfc_bigint_subtract(gfc_bigint_p minuend, gfc_bigint_p subtrahend);

gfc_bigint_p
gfc_bigint_multiply(gfc_bigint_p multiplier, gfc_bigint_p multiplicand);

gfc_bigint_p
gfc_bigint_divide(gfc_bigint_p dividend, gfc_bigint_p divisor);

#ifdef  __cplusplus
}
#endif

#endif /* __GFC_MATH_H__ */
