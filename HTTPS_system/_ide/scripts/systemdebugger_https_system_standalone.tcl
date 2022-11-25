# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: D:\Master\TFM\VitisWorkspace\Software\HTTPS_system\_ide\scripts\systemdebugger_https_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source D:\Master\TFM\VitisWorkspace\Software\HTTPS_system\_ide\scripts\systemdebugger_https_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351B100BCA" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351B100BCA-13722093-0"}
fpga -file D:/Master/TFM/VitisWorkspace/Software/HTTPS/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/Master/TFM/VitisWorkspace/Software/zyboProject/export/zyboProject/hw/design_1_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source D:/Master/TFM/VitisWorkspace/Software/HTTPS/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow D:/Master/TFM/VitisWorkspace/Software/HTTPS/Debug/HTTPS.elf
configparams force-mem-access 0
bpadd -addr &main
