.set noat
.set noreorder

.section .text

# This routine runs from SP IMEM while RDRAM is reset, then enters the newly
# uploaded ROM's IPL3 at the standard SP DMEM address.
practice_sc64_reboot_start:
    .globl practice_sc64_reboot_start

    lui     $t5, 0xBFC0
1:
    lw      $t0, 0x7FC($t5)
    addiu   $t5, $t5, 0x7C0
    andi    $t0, $t0, 0x80
    bnel    $t0, $zero, 1b
     lui    $t5, 0xBFC0
    lw      $t0, 0x24($t5)
    lui     $t3, 0xB000

.set practice_sc64_reboot_entry, 0xA4001000 + (. - practice_sc64_reboot_start)
.globl practice_sc64_reboot_entry

    li      $sp, 0xA4001FF0

    bnez    $a0, 2f
     nop
    bnez    $s5, 2f
     nop
    li      $t0, 0xA4700000
    sw      $zero, 0x10($t0)
    sw      $zero, 0x0C($t0)
2:
    li      $t0, 1
    beq     $s4, $zero, 3f
     nop
    beq     $s4, $t0, 4f
     nop
    li      $ra, 0xA4001554
    b       5f
     nop
3:
    li      $ra, 0xA4001554
    b       5f
     nop
4:
    li      $ra, 0xA4001550
5:
    move    $at, $zero
    move    $v0, $zero
    move    $v1, $zero
    move    $a0, $zero
    move    $a1, $zero
    move    $a2, $zero
    move    $a3, $zero
    move    $t0, $zero
    move    $t1, $zero
    li      $t2, 0x40
    move    $t4, $zero
    move    $t5, $zero
    move    $t6, $zero
    move    $t7, $zero
    move    $s0, $zero
    move    $s1, $zero
    move    $s2, $zero
    move    $t8, $zero
    move    $t9, $zero
    move    $k0, $zero
    move    $k1, $zero
    move    $gp, $zero
    move    $fp, $zero
    li      $t3, 0xA4000040
    jr      $t3
     nop

practice_sc64_reboot_end:
    .globl practice_sc64_reboot_end
