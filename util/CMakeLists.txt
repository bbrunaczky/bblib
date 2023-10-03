set(LIB bb-util)

set(LIB_SOURCE_FILES
    src/empty.cpp
   )

add_library(${LIB} SHARED ${LIB_SOURCE_FILES})
target_include_directories(${LIB} PUBLIC src)

install(TARGETS ${LIB} DESTINATION lib)
install(DIRECTORY src/bb DESTINATION include)

# # # # #

set(TST util-tst)
set(TST_SOURCE_FILES
        tst/util-tst.cpp
    )

add_executable(${TST} ${TST_SOURCE_FILES})
target_include_directories(${TST} PRIVATE tst)
target_link_libraries(${TST}
    ${LIB}
    gtest)

add_test(NAME ${TST}
         COMMAND $<TARGET_FILE:${TST}>
        )
