/* unit_0000051C: .text 0x51C-0xD6C, one translation unit of the game.
 *
 * Bounds: the unit's own .eh_frame CIE (0x1E4B1C) and its five FDEs (0x51C, 0x5B4,
 * 0x64C, 0x964, 0xBD4); the three leaf functions without an FDE lie between them.
 * Its floats are the .rodata.cst4 block at 0x212740 (2.0f, FLT_MAX, 0.5f).
 * Names are unknown: functions and data keep their addresses. The box reading
 * (two vectors, centre and half extents) comes from the code only. */

struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float &operator[](int n) { return (&x)[n]; }
    const float &operator[](int n) const { return (&x)[n]; }
};

struct Box {
    Vec3 a; /* 0x00 */
    Vec3 b; /* 0x0C */
};

extern "C" {

Vec3 func_00009274(const Vec3 &v, float s);
Vec3 func_0000931C(const Vec3 &v, float s);
Vec3 func_00009484(const Vec3 &a, const Vec3 &b);
Vec3 func_000094BC(const Vec3 &a, const Vec3 &b);
void func_000064D8(Vec3 a, Vec3 b, int c);
void func_00006E84(Vec3 a, Vec3 b, int c);

void func_0000051C(Box *o, int c)
{
    func_00006E84(func_00009484(o->a, o->b), func_000094BC(o->a, o->b), c);
}

void func_000005B4(Box *o, int c)
{
    func_000064D8(func_00009484(o->a, o->b), func_000094BC(o->a, o->b), c);
}

void func_0000064C(Box *o, int c, int n)
{
    Vec3 half = func_00009274(o->b, 2.0f);
    Vec3 cell = func_0000931C(half, (float)n);
    Vec3 ext = func_0000931C(cell, 2.0f);
    Vec3 base = func_000094BC(func_00009484(o->a, o->b), ext);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++) {
                Vec3 p = func_000094BC(base, Vec3(cell.x * i, cell[1] * j, cell.z * k));
                func_00006E84(func_00009484(p, ext), func_000094BC(p, ext), c);
            }
}

/* Clips the interval [*t0, *t1] against one plane. */
bool func_000008AC(float denom, float numer, float *t0, float *t1)
{
    if (denom > 0.0f) {
        if (denom * *t1 < numer) {
            return false;
        }
        if (denom * *t0 < numer) {
            *t0 = numer / denom;
        }
        return true;
    } else if (denom < 0.0f) {
        if (denom * *t0 < numer) {
            return false;
        }
        if (denom * *t1 < numer) {
            *t1 = numer / denom;
        }
        return true;
    } else {
        return numer <= 0.0f;
    }
}

struct Clip {
    float t0;
    float t1;
    Vec3 d;
};

bool func_00000964(Box *b, Vec3 *o, Vec3 *dir)
{
    Clip c;
    float v;
    bool r;
    c.d = func_00009484(*o, b->a);
    c.t0 = 0.0f;
    c.t1 = 3.40282347e+38f;
    v = c.d.x;
    if (!func_000008AC(dir->x, -v - b->b.x, &c.t0, &c.t1)) return false;
    if (!func_000008AC(-dir->x, v - b->b.x, &c.t0, &c.t1)) return false;
    v = c.d.y;
    if (!func_000008AC(dir->y, -v - b->b.y, &c.t0, &c.t1)) return false;
    if (!func_000008AC(-dir->y, v - b->b.y, &c.t0, &c.t1)) return false;
    v = c.d.z;
    if (!func_000008AC(dir->z, -v - b->b.z, &c.t0, &c.t1)) return false;
    r = false;
    if (func_000008AC(-dir->z, v - b->b.z, &c.t0, &c.t1)) {
        r = true;
        if (c.t0 == 0.0f && c.t1 == 3.40282347e+38f) r = false;
    }
    return r;
}

/* Overlap of [a - b, a + b] and [c - d, c + d]. */
bool func_00000AF8(float a, float b, float c, float d)
{
    float min1 = a - b;
    float max1 = a + b;
    float min2 = c - d;
    float max2 = c + d;
    return !(min1 > max2 || max1 < min2);
}

bool func_00000B38(Box *a, Box *b)
{
    if (func_00000AF8(a->a.x, a->b.x, b->a.x, b->b.x) &&
        func_00000AF8(a->a.y, a->b.y, b->a.y, b->b.y) &&
        func_00000AF8(a->a.z, a->b.z, b->a.z, b->b.z))
        return true;
    return false;
}

void func_00000BD4(Box *o, Vec3 *p)
{
    Vec3 lo = func_00009484(o->a, o->b);
    Vec3 hi = func_000094BC(o->a, o->b);
    if (hi.x < p->x) hi.x = p->x;
    if (hi.y < p->y) hi.y = p->y;
    if (hi.z < p->z) hi.z = p->z;
    if (p->x < lo.x) lo.x = p->x;
    if (p->y < lo.y) lo.y = p->y;
    if (p->z < lo.z) lo.z = p->z;
    o->b = func_00009274(func_00009484(hi, lo), 0.5f);
    o->a = func_000094BC(lo, o->b);
}

}
