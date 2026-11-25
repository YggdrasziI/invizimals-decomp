/* unit_00000D6C: .text 0xD6C-0x20C0, one translation unit of the game.
 *
 * Bounds: the unit's own .eh_frame CIE (0x1E4BF0). Its strings are the
 * .rodata.str1.4 block that starts at 0x216248, whose first three strings are used
 * by the inline virtual functions of TActionsDialogListener (linkonce copies kept
 * from this unit at 0x1BA0E4-0x1BA464). Its static object is the dialog listener at
 * 0x2481DC: vtables 0x23A260 (TActionsDialogListener) and 0x23A2E8
 * (TPSPDialogs::CPSPDialogsListener), both named by their RTTI strings.
 *
 * The unit runs a list of network setup actions (20-byte entries ended by type
 * 0x1D), one step at a time, forwards and backwards; the readings of the fields
 * come from the code and the game's debug strings only. Names are unknown unless
 * stated: functions and data keep their addresses.
 *
 * NonMatching: func_0000126C and func_000012B4 (a constant multiplication the
 * compiler does not choose yet). */

/* An enum, not int: the stores to `int` fields may then pass its loads (func_0000113C) */
enum ActionType { ACTION_TYPE_MAX = 0x7FFFFFFF };
/* An enum, not int, for the same reason (func_00001E60) */
enum CursorState { CURSOR_IDLE, CURSOR_UNDO, CURSOR_DO, CURSOR_DONE };
struct Action {
    ActionType type;  /* 0x00, 0x1D ends a list */
    int arg;          /* 0x04 */
    const char *name; /* 0x08 */
    int rc;           /* 0x0C */
    int state;        /* 0x10 */
};

struct Cursor {
    Action *entries;   /* 0x00 */
    Action *cur;       /* 0x04 */
    CursorState state; /* 0x08 */
    int count;         /* 0x0C */
    bool done;         /* 0x10 */
    int rc;            /* 0x14 */
};

/* Object at 0x2481E8, constructed by func_00095170 */
struct T_002481E8 {
    char pad[0x10];
    int f10; /* 0x10 */
    T_002481E8(); /* func_00095170 */
};

struct T_00248CE0;

struct StrTable {
    char *base;
    int *offsets;
};

struct GroupName {
    char data[8];
};

struct AdhocId {
    int type;
    char data[9];
};

struct Drawer;
struct DrawerVtbl {
    void *s0;
    void *s1;
    void *s2;
    void (*draw)(Drawer *d, int x, long y, int color, const char *s);
};
struct Drawer {
    DrawerVtbl *vt;
};

extern "C" {
int func_00010360(const char *fmt, ...);
}

namespace TPSPDialogs {

class CPSPDialogsListener {
public:
    virtual ~CPSPDialogsListener() {}
    virtual void vfunc2(int) {}
    virtual void vfunc3(int) {}
    virtual void vfunc4(int) {}
    virtual void vfunc5(int) {}
    virtual void vfunc6(int) {}
    virtual void vfunc7(int) {}
    virtual void vfunc8(int) {}
    virtual void vfunc9(int) {}
};

}

class TActionsDialogListener : public TPSPDialogs::CPSPDialogsListener {
public:
    Action *action; /* 0x04 */

    /* Named by its debug string; inlined in the three handlers (0x1BA144, 0x1BA264,
     * 0x1BA364). Their kept copies place the store of state 3 / 4 after the epilogue,
     * ours at the end (85.7 %, not part of this unit's piece). */
    void onFinishedDialog(int result)
    {
        if (action) {
            func_00010360("onFinishedDialog (%08x) action = %s\n", result, action->name);
            bool ok;
            if (action->type == 0x1B)
                ok = result == 1;
            else if (action->type == 0x1C)
                ok = true;
            else
                ok = result == 0;
            func_00010360("prev result was %d, is_ok= %d\n", action->state, ok);
            if (action->state == 2) {
                if (ok)
                    action->state = 3;
                else
                    action->state = 4;
            }
            func_00010360("action has now result = %d\n", action->state);
            action = 0;
        }
    }

    virtual void vfunc2(int result) { onFinishedDialog(result); }
    virtual void vfunc7(int result) { onFinishedDialog(result); }
    virtual void vfunc8(int result) { onFinishedDialog(result); }
};

extern "C" {

void func_000020F0(void);
int func_000020C0(void);
void func_001AFE98(const char *s);
int sceUtilityNetconfShutdownStart(void);
int sceUtilityNpSigninShutdownStart(void);
int func_00095240(T_002481E8 *p);
void func_00095254(T_002481E8 *p, float dt);
void func_0009524C(T_002481E8 *p, TActionsDialogListener *l);
void func_00095820(T_002481E8 *p, const char *s);
void func_00095870(T_002481E8 *p);
void func_0008E704(void);
int func_00009ACC(T_00248CE0 *p, int a);
int func_000036C8(Drawer *d, int x, long y, int color, const char *fmt, int b, int c);
void *func_0000BF94(void);
void func_0000C8E0(TActionsDialogListener *l);
int func_0000C9BC(const char *s, int b);
int func_0000CE04(void);
int func_0000CFE8(void);
int func_0000BF14(void);

int sceUtilityLoadModule(int);
int sceUtilityUnloadModule(int);
int sceNetInit(int, int, int, int, int);
int sceNetTerm(void);
int sceNetInetInit(void);
int sceNetInetTerm(void);
int sceNetResolverInit(void);
int sceNetResolverTerm(void);
int sceNetAdhocctlInit(int, int, AdhocId *);
int sceNetAdhocctlTerm(void);
int sceNetApctlAddHandler(void (*)(int, int, int, int), void *);
int sceNetApctlDelHandler(int);
int sceNetAdhocctlConnect(GroupName *);
int sceNetAdhocctlDisconnect(void);
int sceNetApctlInit(int, int);
int sceNetApctlTerm(void);
int sceNetAdhocctlAddHandler(void (*)(int, int), void *);
int sceNetAdhocctlDelHandler(int);
int sceNetAdhocInit(void);
int sceNetAdhocTerm(void);
int sceSslInit(int);
int sceSslEnd(void);
int sceHttpInit(int);
int sceHttpEnd(void);
int sceNpGetChatRestrictionFlag(int *);
int sceNpMatching2CreateContext(void *, void *, unsigned short *, int);
int sceNpMatching2DestroyContext(unsigned short);
int sceNpMatching2ContextStart(unsigned short);
int sceNpMatching2ContextStop(unsigned short);
int sceNpInit(void);
int sceNpTerm(void);
int sceNetUpnpInit(int, int);
int sceNetUpnpTerm(void);
int sceNetUpnpStart(void);
int sceNetUpnpStop(void);
int sceNetApctlConnect(int);
int sceNetApctlDisconnect(void);
int sceNpMatching2Init(int, int, int, int);
int sceNpMatching2Term(void);
int sceNpServiceInit(int, int, int);
int sceNpServiceTerm(void);
int sceHttpsInit(int, int, int, int);
int sceHttpsEnd(void);
int sceNpAuthInit(int, int, int);
int sceNpAuthTerm(void);
int sceUtilityNetconfGetStatus(void);
int sceNpGetUserProfile(void *);
int sceKernelDelayThread(int);

extern char *D_00243840[];
extern char D_00243790[][16];
extern char D_002437F0[];
extern char D_0020B848[];
extern char D_0020B7C8[];
extern int D_00243788;
extern int D_0024378C;
extern StrTable D_004CA528;
extern T_00248CE0 D_00248CE0;

extern Action *D_0024831C;
extern unsigned int D_00248320;
extern Action *D_00248324;
extern int D_00248328;
extern int D_0024832C;
extern int D_00248330;
extern unsigned char D_0024833C;
extern bool D_00248240;
extern char D_00248244[];
extern unsigned short *D_002481E4;

T_002481E8 D_002481E8;
TActionsDialogListener D_002481DC;

int func_00000D6C(void)
{
    int v = D_0024832C;
    return v < 0 ? v : 0;
}

int func_00000D80(void)
{
    int v = D_00248328;
    return v < 0 ? v : 0;
}

void func_00000D94(void)
{
    D_0024832C = 0;
}

void func_00000DA0(void)
{
    D_00248328 = 0;
}

/* sceNetApctl handler */
void func_00000DAC(int a, int b, int c, int d)
{
    if (c == 6) {
        func_00010360("event: %s -- %s(0x%x) --> %s\n", D_00243840[a], D_002437F0, d, D_00243840[b]);
    } else {
        func_00010360("event: %s -- %s --> %s\n", D_00243840[a], D_00243790[c], D_00243840[b]);
    }
    D_00248330 = b;
    if (c == 5 && D_0024831C != 0) {
        func_001AFE98("INET disconnected. marking action as resolved ok");
        Action *p = D_0024831C;
        D_0024831C = 0;
        p->state = 3;
    }
    if (b == 0) {
        if (c == 6) {
            func_00010360("Apctl error happened, error = 0x%x\n", d);
            D_0024832C = d;
        }
    } else if (b == 4 && c == 4) {
        D_0024832C = 0;
        func_000020F0();
    }
}

/* sceNetAdhocctl handler */
void func_00000F24(int mode, int arg)
{
    Action *p = D_00248324;
    if (mode == 0) {
        if (p) {
            if (p->state == 2)
                p->state = 4;
            else
                p->state = 1;
        }
        D_00248328 = arg;
        D_00248320 |= 1;
    } else if (mode == 1) {
        if (p && p->state == 2)
            p->state = 3;
        D_00248320 |= 2;
        D_0024833C = 1;
        D_00248328 = 0;
    } else if (mode == 2) {
        D_0024833C = 0;
        if (p && p->state == 2)
            p->state = 3;
        D_00248320 |= 4;
        D_00248328 = 0;
    } else if (mode == 3) {
        D_00248320 |= 8;
        D_00248328 = 0;
    } else if (mode == 4) {
        D_00248320 |= 0x10;
        D_00248328 = 0;
    }
}

void func_00001068(Action *a)
{
    int type = a->type;
    a->rc = 0;
    if (type == 0x11) {
        a->rc = sceUtilityNetconfShutdownStart();
    } else if (type == 0x15) {
        a->rc = sceUtilityNpSigninShutdownStart();
    } else if (type != 0x10) {
        func_00010360("Unsupported update action type %s (%d)\n", a->name, type);
    }
    if (a->rc != 0) {
        func_00010360("cancelWaiting of %s failed rc = %08x\n", a->name, a->rc);
        a->rc = 0;
    }
}

void func_0000113C(Cursor *c)
{
    c->done = 0;
    c->cur = c->entries;
    c->state = CURSOR_IDLE;
    c->count = 0;
    c->rc = 0;
    for (Action *e = c->entries; e->type != 0x1D; e++) {
        e->state = 5;
        c->count++;
    }
}

void func_00001198(Cursor *c)
{
    func_0000113C(c);
    c->state = CURSOR_DO;
}

void func_000011C4(Cursor *c)
{
    int s = c->state;
    if (s == 2) {
        func_00010360("User disconnects. Current action is %s result=%d\n", c->cur->name, c->cur->state);
        if (c->cur->state == s) {
            func_00001068(c->cur);
        } else {
            c->state = CURSOR_UNDO;
        }
    }
}

void func_0000124C(Cursor *c)
{
    if (c->state == 3) {
        c->state = CURSOR_UNDO;
        return;
    }
    func_000011C4(c);
}

float func_0000126C(Cursor *c)
{
    if (c->count == 0)
        return 0.0f;
    return (float)(int)(c->cur - c->entries) / (float)c->count;
}

/* Draws up to three entries of the list, from the current one */
void func_000012B4(Cursor *list, long *y, Drawer *d)
{
    Action *start = list->cur - 1;
    if (start < list->entries)
        start = list->entries;
    for (Action *p = start; p->type != 0x1D; p++) {
        int n = p - start;
        if (n >= 3)
            break;
        int color;
        if (p < list->cur) {
            color = p->rc ? 0xFF0000FF : 0xFF00FF00;
        } else if (p == list->cur) {
            color = p->state == 2 ? 0xFFFF0000 : -1;
        } else {
            color = 0xFF808080;
        }
        d->vt->draw(d, 0x14, *y, color, p->name);
        long y0 = *y;
        *y = y0 + func_000036C8(d, 0x140, y0, color, "%08x %d", p->rc, p->state);
    }
}

void func_00001470(void)
{
    if (func_00095240(&D_002481E8)) {
        func_0008E704();
        func_00095870(&D_002481E8);
    }
}

static inline void SetText(T_002481E8 *t, const char *s)
{
    t->f10 = 0;
    func_00095820(t, s);
}

#define STR(i) (D_004CA528.base + D_004CA528.offsets[i])

/* Runs one action (undo = 0) or its reverse (undo = 1); returns its new state */
int func_000014B8(Action *a, int undo)
{
    a->state = 5;
    a->rc = 0;
    func_00010360("[%p] %s: %s\n", a, !undo ? "Doing" : "Undoing", a->name ? a->name : "<NULL>");

    switch (a->type) {
    case 0:
    case 29:
        break;
    case 1:
        if (!undo)
            a->rc = sceUtilityLoadModule(a->arg);
        else
            a->rc = sceUtilityUnloadModule(a->arg);
        break;
    case 2:
        if (!undo)
            a->rc = sceNetInit(0x20000, 0x28, 0, 0x28, 0);
        else
            a->rc = sceNetTerm();
        break;
    case 3:
        if (!undo)
            a->rc = sceNetInetInit();
        else
            a->rc = sceNetInetTerm();
        break;
    case 4:
        if (!undo)
            a->rc = sceNetResolverInit();
        else
            a->rc = sceNetResolverTerm();
        break;
    case 5:
        if (!undo)
            a->rc = sceNetApctlInit(0x4800, 0x30);
        else
            a->rc = sceNetApctlTerm();
        break;
    case 6:
        if (!undo) {
            a->rc = sceNetApctlAddHandler(func_00000DAC, 0);
            if (a->rc < 0) {
                D_0024378C = -1;
            } else {
                D_0024378C = a->rc;
                a->rc = 0;
            }
        } else if (D_0024378C != -1) {
            sceNetApctlDelHandler(D_0024378C);
            D_0024378C = -1;
        }
        break;
    case 7:
        if (!undo)
            a->rc = sceNetAdhocInit();
        else
            a->rc = sceNetAdhocTerm();
        break;
    case 8:
        if (!undo) {
            AdhocId id;
            id.type = 0;
            __builtin_memcpy(id.data, func_0000BF94(), 9);
            a->rc = sceNetAdhocctlInit(0x1000, 0x1E, &id);
        } else {
            a->rc = sceNetAdhocctlTerm();
        }
        break;
    case 9:
        if (!undo) {
            a->rc = sceNetAdhocctlAddHandler(func_00000F24, 0);
            if (a->rc < 0) {
                D_00243788 = -1;
            } else {
                D_00243788 = a->rc;
                a->rc = 0;
            }
        } else if (D_00243788 != -1) {
            a->rc = sceNetAdhocctlDelHandler(D_00243788);
            D_00243788 = -1;
        } else {
            a->rc = 0;
        }
        break;
    case 10:
        if (!undo) {
            GroupName g;
            D_00248324 = a;
            D_00248320 = 0;
            __builtin_memset(&g, 0, sizeof(g));
            __builtin_strcpy(g.data, "invizimals");
            a->rc = sceNetAdhocctlConnect(&g);
            if (a->rc == 0)
                a->state = 2;
        } else {
            a->state = 2;
            a->rc = sceNetAdhocctlDisconnect();
        }
        break;
    case 11:
        if (!undo)
            a->rc = sceSslInit(0x25800);
        else
            a->rc = sceSslEnd();
        break;
    case 12:
        if (!undo)
            a->rc = sceHttpInit(0x7800);
        else
            a->rc = sceHttpEnd();
        break;
    case 13:
        if (!undo)
            a->rc = sceHttpsInit(0, 0, 0, 0);
        else
            a->rc = sceHttpsEnd();
        break;
    case 14:
        if (!undo)
            a->rc = sceNetUpnpInit(0x2000, 0x64);
        else
            a->rc = sceNetUpnpTerm();
        break;
    case 15:
        if (!undo)
            a->rc = sceNetUpnpStart();
        else
            a->rc = sceNetUpnpStop();
        break;
    case 16:
        if (!undo) {
            a->rc = sceNetApctlConnect(1);
            func_00010360("sceNetApctlConnect rc = %08x\n", a->rc);
            if (a->rc == 0)
                a->state = 2;
        } else {
            a->rc = sceNetApctlDisconnect();
            a->state = 2;
        }
        break;
    case 17:
        if (!undo) {
            D_002481DC.action = a;
            func_0000C8E0(&D_002481DC);
            a->rc = func_0000CE04();
            if (a->rc != 0) {
                func_00010360("sceUtilityNetconfInitStart failed %08x -> sceUtilityNetconfGetStatus = %d\n",
                              a->rc, sceUtilityNetconfGetStatus());
                if (sceUtilityNetconfGetStatus() == 2)
                    a->rc = 0;
            }
            D_0024831C = 0;
            a->state = 2;
        } else {
            D_0024831C = a;
            a->rc = sceNetApctlDisconnect();
            a->state = 2;
        }
        break;
    case 18:
        if (!undo)
            a->rc = sceNpInit();
        else
            sceNpTerm();
        break;
    case 19:
        if (!undo)
            a->rc = sceNpAuthInit(0x4000, 0x3C00, 0x28);
        else
            sceNpAuthTerm();
        break;
    case 20:
        if (!undo)
            a->rc = sceNpServiceInit(0x4000, 0x4000, 0x28);
        else
            sceNpServiceTerm();
        break;
    case 21:
        if (!undo) {
            D_002481DC.action = a;
            func_0000C8E0(&D_002481DC);
            a->rc = func_0000CFE8();
            a->state = 2;
        }
        break;
    case 22:
        if (!undo) {
            a->rc = sceNpGetUserProfile(D_00248244);
            D_00248240 = a->rc == 0;
        } else {
            D_00248240 = false;
        }
        break;
    case 23:
        if (!undo) {
            a->rc = sceNpMatching2Init(0x20000, 0x28, 0x4000, 0x28);
        } else {
            sceNpMatching2Term();
            sceKernelDelayThread(1000000);
        }
        break;
    case 24:
        if (!undo)
            a->rc = sceNpMatching2CreateContext(D_0020B848, D_0020B7C8, D_002481E4, 1);
        else
            a->rc = sceNpMatching2DestroyContext(*D_002481E4);
        break;
    case 25:
        if (!undo)
            a->rc = sceNpMatching2ContextStart(*D_002481E4);
        else
            a->rc = sceNpMatching2ContextStop(*D_002481E4);
        break;
    case 26:
        if (!undo)
            a->rc = func_000020C0() ? (int)0x80410A06 : 0;
        else
            a->rc = 0;
        break;
    case 27:
        if (!undo) {
            const char *s;
            D_002481DC.action = a;
            func_0009524C(&D_002481E8, &D_002481DC);
            s = STR(D_002481DC.action->arg);
            if (D_002481DC.action->arg == 0x38 && func_0000BF14() == 2)
                s = STR(61);
            else if (D_002481DC.action->arg == 0x38 && func_0000BF14() == 1)
                s = STR(62);
            SetText(&D_002481E8, s);
            a->state = 2;
            a->rc = 0;
        }
        break;
    case 28:
        if (!undo) {
            int flag;
            a->rc = 0;
            flag = 0;
            if (sceNpGetChatRestrictionFlag(&flag) != 0)
                break;
            if (flag != 1)
                break;
            D_002481DC.action = a;
            func_0000C8E0(&D_002481DC);
            a->rc = func_0000C9BC(STR(D_002481DC.action->arg), 0);
            a->state = 2;
        }
        break;
    default:
        func_00010360("Unsupported execute action type %s (%d)\n", a->name, a->type);
        break;
    }

    if (a->rc < 0) {
        func_00010360("%s failed rc = %08x\n", a->name, a->rc);
        a->state = 1;
    } else if (a->state == 5) {
        a->state = 0;
    }
    return a->state;
}

/* One step of the list: forwards (state 2) or backwards (state 1); true when idle */
bool func_00001E60(Cursor *o, float dt)
{
    if (func_00095240(&D_002481E8)) {
        func_00095254(&D_002481E8, dt);
    }
    o->done = 0;
    int st = o->state;
    if (st == 2) {
        Action *n = o->cur;
        int s = n->state;
        if (s == 2) {
        } else if (s == 4) {
            n->state = 1;
            o->state = CURSOR_UNDO;
        } else if (s == 3) {
            o->cur = n + 1;
            n->state = 0;
        } else if (n->type != 0x1D) {
            if (!func_000014B8(n, 0)) {
                o->cur = o->cur + 1;
            } else {
                n = o->cur;
                if (n->state == 1) {
                    o->rc = n->rc;
                    o->state = CURSOR_UNDO;
                }
            }
        } else {
            o->done = 1;
            o->state = CURSOR_DONE;
        }
    } else if (st == 1) {
        Action *n = o->cur;
        int s = n->state;
        if (s == 2) {
            if (func_00009ACC(&D_00248CE0, 1)) {
                o->cur->state = 3;
            }
        } else {
            if (s == 3) {
                n->state = 0;
            }
            o->cur--;
            if (o->cur == o->entries) {
                o->state = CURSOR_IDLE;
                o->done = 1;
            } else {
                func_000014B8(o->cur, 1);
            }
        }
    }
    return o->state != 1 && o->state != 2;
}

unsigned char func_000020B4(void)
{
    return D_0024833C;
}


}
