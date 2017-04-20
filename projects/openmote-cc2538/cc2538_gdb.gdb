target remote localhost:2331
monitor interface jtag
monitor endian little
monitor speed 5000
monitor flash device = CC2538SF53
monitor flash breakpoints = 1
monitor flash download = 1
monitor reset
monitor halt
file build/openmote-cc2538_armgcc/projects/common/01bsp_leds_prog
load build/openmote-cc2538_armgcc/projects/common/01bsp_leds_prog
