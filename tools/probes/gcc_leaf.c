/* Compiler probe: small functions of the game written back in C, compared
 * function by function with the original by tools/psp/compiler_probe.py.
 * Struct layouts are only what each function shows (offsets), nothing more. */

typedef struct { int v[4]; } Quad;

/* 0x00018CD0: six fields cleared, the first one set to -1 */
typedef struct { int a, b, c, d; char e, f; } S18CD0;
void func_00018CD0(S18CD0 *p)
{
    p->a = -1;
    p->b = 0;
    p->c = 0;
    p->d = 0;
    p->e = 0;
    p->f = 0;
}

/* 0x000850DC: a float from .rodata, bytes and words cleared */
extern const float D_00213574;
typedef struct { char a, b; float f; int g, h; } S850DC;
void func_000850DC(S850DC *p)
{
    p->a = 0;
    p->b = 0;
    p->f = D_00213574;
    p->g = 0;
    p->h = 0;
}

/* 0x0008B5B4: three words cleared, then 300 and 600 */
void func_0008B5B4(int *p)
{
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 300;
    p[4] = 600;
}

/* 0x001203B8: counter at 0x1A0, flag byte at 0x14940 */
typedef struct { char pad0[0x1A0]; int count; char pad1[0x14940 - 0x1A4]; char flag; } S1203B8;
void func_001203B8(S1203B8 *p)
{
    p->flag = 1;
    p->count++;
}

/* 0x00120D34: address of element i (0xF4 bytes) of the array at 0x2B88 */
typedef struct { char data[0xF4]; } E120D34;
typedef struct { char pad[0x2B88]; E120D34 *items; } S120D34;
E120D34 *func_00120D34(S120D34 *p, int i)
{
    return &p->items[i];
}

/* 0x001A7D5C: a buffer address when a global is zero */
extern int D_002479E4;
extern char D_006513D4[];
char *func_001A7D5C(void)
{
    if (D_002479E4 != 0)
        return 0;
    return D_006513D4;
}

/* 0x0001797C: tail call with the child at 0x14 */
typedef struct S1797C { char pad[0x14]; struct S1797C *child; } S1797C;
extern void func_00017928(S1797C *child, S1797C *parent);
void func_0001797C(S1797C *p)
{
    if (p->child != 0)
        func_00017928(p->child, p);
}

/* 0x001DA3B0: 16-byte struct copied to 0x50 */
typedef struct { char pad[0x50]; Quad q; } S1DA3B0;
void func_001DA3B0(S1DA3B0 *p, Quad *src)
{
    p->q = *src;
}

/* 0x001DB5EC: element i of a 16-byte array at 0x70 copied out */
typedef struct { char pad[0x70]; Quad q[1]; } S1DB5EC;
void func_001DB5EC(S1DB5EC *p, int i, Quad *out)
{
    *out = p->q[i];
}

/* 0x00006398: tail call with a fifth argument in $t0 (EABI) */
extern char D_002451C0[];
extern void func_001998F8(int, int, int, int, char *);
void func_00006398(void)
{
    func_001998F8(3, 0xA1, 0x24, 0, D_002451C0);
}

/* 0x00008150: length of a 2D float vector, sqrt.s inline */
extern float sqrtf(float);
float func_00008150(float *v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

/* 0x0000124C: state 3 becomes 1, otherwise a tail call */
typedef struct { int a, b, state; } S124C;
extern void func_000011C4(S124C *p);
void func_0000124C(S124C *p)
{
    if (p->state == 3) {
        p->state = 1;
        return;
    }
    func_000011C4(p);
}
