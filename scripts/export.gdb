set confirm off

define dump_memory_to_file
	print i
    set $assembledImagePtr = assembledImagePtr
    set $assembledImageSize = assembledImageSize 
    set $endExportedMemory = $assembledImagePtr + $size   
    if i == 0
        dump ihex memory scripts/memory0.hex $assembledImagePtr $endExportedMemory
	end
    if i == 1
        dump ihex memory scripts/memory1.hex $assembledImagePtr $endExportedMemory
	end
    if i == 2
        dump ihex memory scripts/memory2.hex $assembledImagePtr $endExportedMemory
	end
    if i == 3
        dump ihex memory scripts/memory3.hex $assembledImagePtr $endExportedMemory
	end
    if i == 4
        dump ihex memory scripts/memory4.hex $assembledImagePtr $endExportedMemory
	end
    if i == 5
        dump ihex memory scripts/memory5.hex $assembledImagePtr $endExportedMemory
	end
    if i == 6
        dump ihex memory scripts/memory6.hex $assembledImagePtr $endExportedMemory
	end
    if i == 7
        dump ihex memory scripts/memory7.hex $assembledImagePtr $endExportedMemory
	end
    if i == 8
        dump ihex memory scripts/memory8.hex $assembledImagePtr $endExportedMemory
	end
    if i == 9
        dump ihex memory scripts/memory9.hex $assembledImagePtr $endExportedMemory
	end
    if i == 10
        dump ihex memory scripts/memory10.hex $assembledImagePtr $endExportedMemory
    end
	continue
end

dump_memory_to_file