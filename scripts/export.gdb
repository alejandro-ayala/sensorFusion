set confirm off

define dump_memory_to_file
	print i
    set $ptrFullImg = ptrFullImg
    set $size = fullImgSize 
    set $endExportedMemory = $ptrFullImg + $size   
    if i == 0
        dump ihex memory scripts/memory0.hex $ptrFullImg $endExportedMemory
	end
    if i == 1
        dump ihex memory scripts/memory1.hex $ptrFullImg $endExportedMemory
	end
    if i == 2
        dump ihex memory scripts/memory2.hex $ptrFullImg $endExportedMemory
	end
    if i == 3
        dump ihex memory scripts/memory3.hex $ptrFullImg $endExportedMemory
	end
    if i == 4
        dump ihex memory scripts/memory4.hex $ptrFullImg $endExportedMemory
	end
    if i == 5
        dump ihex memory scripts/memory5.hex $ptrFullImg $endExportedMemory
	end
    if i == 6
        dump ihex memory scripts/memory6.hex $ptrFullImg $endExportedMemory
	end
    if i == 7
        dump ihex memory scripts/memory7.hex $ptrFullImg $endExportedMemory
	end
    if i == 8
        dump ihex memory scripts/memory8.hex $ptrFullImg $endExportedMemory
	end
    if i == 9
        dump ihex memory scripts/memory9.hex $ptrFullImg $endExportedMemory
	end
    if i == 10
        dump ihex memory scripts/memory10.hex $ptrFullImg $endExportedMemory
    end
	continue
end

dump_memory_to_file