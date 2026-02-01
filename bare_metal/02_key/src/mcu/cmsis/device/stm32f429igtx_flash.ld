/**
 * @file STM32F429IGTX_FLASH.ld
 * @brief Linker script for STM32F429IGTX
 * @author QingTeng
 * @version 1.0.0
 * @date 2025-01-18
 * 
 */

ENTRY(Reset_Handler)

_Min_Stack_Size = 0x800;

MEMORY
{
    CCMRAM    (xrw)   : ORIGIN = 0x10000000,   LENGTH = 64K
    RAM       (xrw)   : ORIGIN = 0x20000000,   LENGTH = 192K
    ROM       (rx)    : ORIGIN = 0x08000000,   LENGTH = 1024K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >ROM

  .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);
        _etext = .;
    } >ROM

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >ROM

    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> ROM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
      _ebss = .;
    } >RAM

    __stack_bottom = ORIGIN(RAM) + LENGTH(RAM) - _Min_Stack_Size;
    .stack __stack_bottom :
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
        PROVIDE(_estack = .);
    } >RAM

    /*
    /DISCARD/ :
    {
        *(.ARM.attributes)
        *(.debug_*)
        *(.comment)
    }
    */
}