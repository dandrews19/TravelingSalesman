# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
	TSP.h
	SrcMain.h
)

set(SOURCE_FILES
	TSP.cpp
	SrcMain.cpp
)

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})
