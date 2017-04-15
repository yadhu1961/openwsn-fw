target remote localhost:2331
monitor interface JTAG
monitor endian little
monitor speed auto
monitor flash device = CC2538SF53
monitor flash breakpoints = 1
monitor flash download = 1
monitor reset
monitor halt
load build/openmote-cc2538_armgcc/projects/common/01bsp_leds_prog





