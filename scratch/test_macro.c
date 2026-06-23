#define IF_VA_1(a) a
#define IF_VA_0(a)
#define _IF_VA(cond, a) IF_VA_##cond(a)
#define IF_VA(cond) _IF_VA(cond)

#define CAT(a, b) a##b
#define PROBE(x) x, 1
#define IS_PROBE(x) _IS_PROBE(x)
#define _IS_PROBE(a, b, ...) b
#define NOT(x) CAT(NOT_, x)
#define NOT_0 1
#define NOT_1 0

#define IS_EMPTY(x)  _IS_EMPTY(x)
#define _IS_EMPTY(x) IS_PROBE(CAT(_IS_EMPTY, _##x##_))
#define _IS_EMPTY__  PROBE(~)

#define EXPAND_ARGS_STACK(...) __VA_ARGS__

#define TRYSidestepping(GOTOLABEL)  \
                    AI_TRYSidestepping  ,  \
                    GOTOLABEL ,

#define TRYFiringRoll(GOTOLABEL)  \
                    AI_TRYFiringRoll  ,  \
                    GOTOLABEL ,

#define SWITCH_REAL(VAR, \
               CASE0, CASE_VAL0, CASE_CONTENT0,\
               CASE1, CASE_VAL1, CASE_CONTENT1,\
               CASE2, CASE_VAL2, CASE_CONTENT2,\
               CASE3, CASE_VAL3, CASE_CONTENT3,\
               CASE4, CASE_VAL4, CASE_CONTENT4,\
               CASE5, CASE_VAL5, CASE_CONTENT5,\
               CASE6, CASE_VAL6, CASE_CONTENT6,\
               CASE7, CASE_VAL7, CASE_CONTENT7,\
               CASE8, CASE_VAL8, CASE_CONTENT8,\
               CASE9, CASE_VAL9, CASE_CONTENT9,\
               CASEA, CASE_VALA, CASE_CONTENTA,\
               CASEB, CASE_VALB, CASE_CONTENTB,\
               CASEC, CASE_VALC, CASE_CONTENTC,\
               CASED, CASE_VALD, CASE_CONTENTD,\
               CASEE, CASE_VALE, CASE_CONTENTE,\
               CASEF, CASE_VALF, CASE_CONTENTF)\
VAR \
IF_VA(NOT(IS_EMPTY(CASEF)))(0}; Error: Switch Limited to 15 elements + 1 default u8 CASEF[] = {)\
IF_VA(NOT(IS_EMPTY(CASEF)))(                                                    CASEE (CASE_VALE,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTF)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASEE)))(IF_VA(NOT(IS_EMPTY(CASEF)))(Label(lblNext))         CASED (CASE_VALD,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTE)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASED)))(IF_VA(NOT(IS_EMPTY(CASEE)))(Label(lblNext))         CASEC (CASE_VALC,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTD)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASEC)))(IF_VA(NOT(IS_EMPTY(CASED)))(Label(lblNext))         CASEB (CASE_VALB,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTC)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASEB)))(IF_VA(NOT(IS_EMPTY(CASEC)))(Label(lblNext))         CASEA (CASE_VALA,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTB)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASEA)))(IF_VA(NOT(IS_EMPTY(CASEB)))(Label(lblNext))         CASE9 (CASE_VAL9,lblNext) EXPAND_ARGS_STACK(CASE_CONTENTA)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE9)))(IF_VA(NOT(IS_EMPTY(CASEA)))(Label(lblNext))         CASE8 (CASE_VAL8,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT9)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE8)))(IF_VA(NOT(IS_EMPTY(CASE9)))(Label(lblNext))         CASE7 (CASE_VAL7,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT8)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE7)))(IF_VA(NOT(IS_EMPTY(CASE8)))(Label(lblNext))         CASE6 (CASE_VAL6,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT7)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE6)))(IF_VA(NOT(IS_EMPTY(CASE7)))(Label(lblNext))         CASE5 (CASE_VAL5,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT6)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE5)))(IF_VA(NOT(IS_EMPTY(CASE6)))(Label(lblNext))         CASE4 (CASE_VAL4,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT5)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE4)))(IF_VA(NOT(IS_EMPTY(CASE5)))(Label(lblNext))         CASE3 (CASE_VAL3,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT4)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE3)))(IF_VA(NOT(IS_EMPTY(CASE4)))(Label(lblNext))         CASE2 (CASE_VAL2,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT3)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE2)))(IF_VA(NOT(IS_EMPTY(CASE3)))(Label(lblNext))         CASE1 (CASE_VAL1,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT2)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE1)))(IF_VA(NOT(IS_EMPTY(CASE2)))(Label(lblNext))         CASE0 (CASE_VAL0,lblNext) EXPAND_ARGS_STACK(CASE_CONTENT1)(lblDone))\
IF_VA(NOT(IS_EMPTY(CASE0)))(IF_VA(NOT(IS_EMPTY(CASE1)))(Label(lblNext))                                   EXPAND_ARGS_STACK(CASE_CONTENT0)(lblDone) \
Label(lblDone))

#define SWITCH_PAD(VAR, \
               C0, V0, CT0, C1, V1, CT1, C2, V2, CT2, C3, V3, CT3, \
               C4, V4, CT4, C5, V5, CT5, C6, V6, CT6, C7, V7, CT7, \
               C8, V8, CT8, C9, V9, CT9, CA, VA, CTA, CB, VB, CTB, \
               CC, VC, CTC, CD, VD, CTD, CE, VE, CTE, CF, VF, CTF, ...) \
               SWITCH_REAL(VAR, \
               C0, V0, CT0, C1, V1, CT1, C2, V2, CT2, C3, V3, CT3, \
               C4, V4, CT4, C5, V5, CT5, C6, V6, CT6, C7, V7, CT7, \
               C8, V8, CT8, C9, V9, CT9, CA, VA, CTA, CB, VB, CTB, \
               CC, VC, CTC, CD, VD, CTD, CE, VE, CTE, CF, VF, CTF)

#define SWITCH(VAR, ...) SWITCH_PAD(VAR, __VA_ARGS__, \
               ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, )

SWITCH(SetNewRandom(),
    IFRandomGreaterThan, 60, TRYFiringRoll,
    IFRandomGreaterThan, 10, TRYSidestepping
)
