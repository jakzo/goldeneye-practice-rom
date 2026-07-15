#include <ultra64.h>
#include <bondtypes.h>

#define ABI_ASSERT(name, expression) typedef char abi_assert_##name[(expression) ? 1 : -1]

enum AbiEnumWidthCheck { ABI_ENUM_ZERO, ABI_ENUM_LARGE = 0x7fffffff };

struct AbiAlignMtx { char prefix; Mtx value; };
struct AbiAlignCoord3d { char prefix; coord3d value; };
struct AbiAlignProp { char prefix; PropRecord value; };

ABI_ASSERT(s8_size, sizeof(s8) == 1);
ABI_ASSERT(u16_size, sizeof(u16) == 2);
ABI_ASSERT(s32_size, sizeof(s32) == 4);
ABI_ASSERT(u64_size, sizeof(u64) == 8);
ABI_ASSERT(pointer_size, sizeof(void *) == 4);
ABI_ASSERT(float_size, sizeof(f32) == 4);
ABI_ASSERT(double_size, sizeof(f64) == 8);
ABI_ASSERT(unsigned_plain_char, (char)-1 > 0);
ABI_ASSERT(enum_width, sizeof(enum AbiEnumWidthCheck) == 4);

ABI_ASSERT(mtx_size, sizeof(Mtx) == 64);
ABI_ASSERT(mtx_alignment, sizeof(struct AbiAlignMtx) == 72);
ABI_ASSERT(coord3d_size, sizeof(coord3d) == 12);
ABI_ASSERT(coord3d_alignment, sizeof(struct AbiAlignCoord3d) == 16);
ABI_ASSERT(prop_size, sizeof(PropRecord) == 0x34);
ABI_ASSERT(prop_alignment, sizeof(struct AbiAlignProp) == 0x38);

#ifdef __GNUC__
ABI_ASSERT(prop_pos_offset, __builtin_offsetof(PropRecord, pos) == 0x08);
ABI_ASSERT(prop_stan_offset, __builtin_offsetof(PropRecord, stan) == 0x14);
ABI_ASSERT(prop_rooms_offset, __builtin_offsetof(PropRecord, rooms) == 0x2c);
#endif

ABI_ASSERT(message_queue_size, sizeof(OSMesgQueue) == 0x18);
#ifdef __GNUC__
ABI_ASSERT(message_queue_msg_offset, __builtin_offsetof(OSMesgQueue, msg) == 0x14);
#endif
ABI_ASSERT(timer_size, sizeof(OSTimer) == 0x20);
#ifdef __GNUC__
ABI_ASSERT(timer_interval_offset, __builtin_offsetof(OSTimer, interval) == 0x08);
ABI_ASSERT(timer_mq_offset, __builtin_offsetof(OSTimer, mq) == 0x18);
#endif

int abi_checks_compile_only;
