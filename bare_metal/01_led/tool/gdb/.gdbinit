## define
# inital
define my_init
    # set
    set pagination off
    set confirm off
    set height 0
    set width 0
    set verbose off
    set logging file gdb_debug.txt
    set logging overwrite on
    set logging enable on
    # connect
    target extended-remote localhost:3333
    monitor reset halt
    load
    printf "[my_init] Done!"
end
# quit
define my_quit
    set logging enable off
    quit
end
# peripheral
# gpiob
define gpiob
    printf "[GPIOB reg]\n"
    set $base = 0x40020400
    printf "MODER:   0x%08x\n", *(uint32_t*)($base + 0x00)
    printf "OTYPER:  0x%08x\n", *(uint32_t*)($base + 0x04)
    printf "OSPEEDR: 0x%08x\n", *(uint32_t*)($base + 0x08)
    printf "PUPDR:   0x%08x\n", *(uint32_t*)($base + 0x0C)
    printf "IDR:     0x%08x\n", *(uint32_t*)($base + 0x10)
    printf "ODR:     0x%08x\n", *(uint32_t*)($base + 0x14)
    printf "BSRR:    0x%08x\n", *(uint32_t*)($base + 0x18)
    printf "LCKR:    0x%08x\n", *(uint32_t*)($base + 0x1C)
    printf "AFRL:    0x%08x\n", *(uint32_t*)($base + 0x20)
    printf "AFRH:    0x%08x\n", *(uint32_t*)($base + 0x24)
end
# RCC
define rcc
    printf "[RCC reg]\n"
    set $base = 0x40023800
    printf "CR:       0x%08x\n", *(uint32_t*)($base + 0x00)
    printf "PLLCFGR:  0x%08x\n", *(uint32_t*)($base + 0x04)
    printf "CFGR:     0x%08x\n", *(uint32_t*)($base + 0x08)
    printf "AHB1ENR:  0x%08x\n", *(uint32_t*)($base + 0x30)
    printf "AHB2ENR:  0x%08x\n", *(uint32_t*)($base + 0x34)
    printf "AHB3ENR:  0x%08x\n", *(uint32_t*)($base + 0x38)
    printf "APB1ENR:  0x%08x\n", *(uint32_t*)($base + 0x40)
    printf "APB2ENR:  0x%08x\n", *(uint32_t*)($base + 0x44)
end
# LED
define led_off
    # GPIOB_BSRR set(off) bit0/1
    set $base = 0x40020400
    set $bit = 0
    set *(uint32_t*)($base + 0x18) = (1 << $bit)
    printf "[LED on]\n"
end
#
define led_on
    # GPIOB_BSRR reset(on) bit0(ds1)/1(ds0)
    set $base = 0x40020400
    set $bit = 0
    set *(uint32_t*)($base + 0x18) = (1 << ($bit  + 16))
    printf "[LED off]\n"
end
#
define led_toggle
    set $base = 0x40020400
    set $odr = *(uint32_t*)($base + 0x14)
    if $odr & (1 << 13)
        led_off
    else
        led_on
    end
end
# DEBUG
define my_debug
    break main
    continue
    # rcc
    # gpiob
    led_on
    led_off
    led_toggle
    printf "[my_debug]\n"
end

## flow
my_init

my_debug

# my_quit