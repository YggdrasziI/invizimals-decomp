/* unit_00018CD0: .text 0x18CD0-0x18CF0, func_00018CD0 alone.
 *
 * Bounds: hypothesis. The unit holds one function cut at its own start and end;
 * the translation unit it came from is not known. Struct layout: only the
 * offsets this function writes. */

typedef struct {
    int a;  /* 0x00 */
    int b;  /* 0x04 */
    int c;  /* 0x08 */
    int d;  /* 0x0C */
    char e; /* 0x10 */
    char f; /* 0x11 */
} S18CD0;

void func_00018CD0(S18CD0 *p)
{
    p->a = -1;
    p->b = 0;
    p->c = 0;
    p->d = 0;
    p->e = 0;
    p->f = 0;
}
