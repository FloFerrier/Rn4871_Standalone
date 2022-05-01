set(SOURCE_FILES
	# Put yours source and header files here !
	)

add_library(target STATIC ${SOURCE_FILES})

set_target_properties(target PROPERTIES LINK_FLAGS "--coverage")