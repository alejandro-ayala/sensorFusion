# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\Alex\Desktop\cryptoProject\zyboProject\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\Alex\Desktop\cryptoProject\zyboProject\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

set currentDir [pwd]
set hwFileDir ${currentDir}9876
platform create -name {zyboProject}\
-hw $currentDir/../HWFile/design_1_wrapper.xsa\
-proc {ps7_cortexa9_0} -os {freertos10_xilinx} -out $currentDir/..

platform write
platform generate -domains 
platform active {zyboProject}
domain active {zynq_fsbl}
bsp reload
domain active {freertos10_xilinx_domain}
bsp reload
bsp setlib -name lwip211 -ver 1.7
bsp config api_mode "SOCKET_API"
bsp config message_buffer true
bsp write
bsp reload
catch {bsp regenerate}
platform generate
platform generate -domains freertos10_xilinx_domain 
platform generate -domains freertos10_xilinx_domain 
platform generate
platform generate
bsp config lwip_dhcp "true"
bsp write
bsp reload
