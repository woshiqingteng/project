## define
# inital
define my_init
    # set
    set pagination off
    set confirm off
    set height 0
    set width 0
    set verbose off
    # timestamp
    python
import time
timestamp = time.strftime("%y%m%d_%H%M%S")
gdb.execute(f'set $ts = "{timestamp}"')
gdb.execute(f"set logging file debug_{timestamp}.txt")
    end
    #set logging file debug_timestamp.txt
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
# gpioa
define gpioa
    printf "[GPIOA reg]\n"
    set $base = 0x40020000
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
define led_on
    # GPIOG_BSRR set bit13
    set *(uint32_t*)0x40021814 = (1 << 13)
    printf "[LED on]\n"
end
#
define led_off
    # GPIOG_BSRR reset bit13
    set *(uint32_t*)0x40021814 = (1 << (13 + 16))  
    printf "[LED off]\n"
end
#
define led_toggle
    set $odr = *(uint32_t*)0x40021814
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
    rcc
    gpioa
    led_on
    led_off
    led_toggle
    printf "[my_debug]\n"
end

## flow
my_init

my_debug

my_quit