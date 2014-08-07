cmake_minimum_required (VERSION 2.8) 
project (microsupport)
enable_testing()

set(LIBS ${LIBS} ${CHECK_LIBRARIES} microsupport)

include_directories( include )

file(GLOB_RECURSE MICROSUPPORT_INCLUDES "include/*.h" "include/*/*.h"  )

if(APPLE)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/win32/*.c" )
endif(APPLE)
if(UNIX)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/posix/*.c"  "src/posix/*.h" )
endif(UNIX)
if(WIN32)
file(GLOB_RECURSE MICROSUPPORT_SRC "src/*.c" "src/macosx/*.c" )
endif(WIN32)

add_library (microsupport ${MICROSUPPORT_SRC} ${MICROSUPPORT_INCLUDES})

link_directories( ${MICROSUPPORT_BINARY_DIR} )

file(GLOB MICROSUPPORT_TESTS "tests/*.c")
foreach(item ${MICROSUPPORT_TESTS})
  GET_FILENAME_COMPONENT(testname ${item} NAME_WE )
  add_executable(${testname} ${item})
  target_link_libraries(${testname} ${LIBS} )
  add_test(NAME ${testname} COMMAND ${testname} )
endforeach(item)

file(GLOB MICROSUPPORT_EXAMPLES "examples/*.c")
foreach(item ${MICROSUPPORT_EXAMPLES})
  GET_FILENAME_COMPONENT(examplename ${item} NAME_WE )
  add_executable(${examplename} ${item})
  target_link_libraries(${examplename} ${LIBS} )
endforeach(item)


INSTALL(TARGETS microsupport DESTINATION "lib" EXPORT microsupport-exports.cmake)

EXPORT(TARGETS microsupport FILE microsupport-exports.cmake)

INSTALL(FILES
        ${MICROSUPPORT_INCLUDES}
        DESTINATION include        
)

INSTALL(EXPORT microsupport-exports.cmake DESTINATION "lib")

