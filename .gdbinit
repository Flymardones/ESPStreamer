#define target remote
#target extended-remote $arg0
#end





#define espreset
target remote :3333
set remote hardware-watchpoint-limit 2
monitor reset halt 

mon program_esp build/bootloader/bootloader.bin 0x1000 verify
mon program_esp build/partition_table/partition-table.bin 0x8000 verify
mon program_esp build/hello_world.bin 0x10000 verify

monitor reset halt
flushregs
thb app_main
c
#end









#define target hookpost-remote
#shell sleep 1
#thb app_main
#c
#end







#target remote :3333


#define target hookpost-remote

#set verbose on
#set history save on
#enable breakpoint
#set remote hardware-watchpoint-limit 2
#monitor reset halt
#flushregs

#thb app_main
