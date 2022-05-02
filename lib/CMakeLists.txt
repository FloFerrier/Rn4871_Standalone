set(SOURCE_FILES
	rn4871-driver/rn4871_defs.h
	rn4871-driver/rn4871.c
	rn4871-driver/rn4871.h
	rn4871-driver/virtual_module.c
	rn4871-driver/virtual_module.h
	rn4871-driver/gatt.h)

add_library(target STATIC ${SOURCE_FILES})

set_target_properties(target PROPERTIES LINK_FLAGS "--coverage")