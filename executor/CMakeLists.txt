set(LIB bb-executor)

set(LIB_SOURCE_FILES
    src/executor.cpp
    src/threadpool.cpp
   )

add_library(${LIB} SHARED ${LIB_SOURCE_FILES})
target_include_directories(${LIB} PUBLIC src)

install(TARGETS ${LIB} DESTINATION lib)
install(DIRECTORY src/bb DESTINATION include)

# # # # #

set(TST executor-tst)
set(TST_SOURCE_FILES
        tst/executor-tst.cpp
    )

add_executable(${TST} ${TST_SOURCE_FILES})
target_link_libraries(${TST}
    ${LIB}
    gtest)

add_test(NAME ${TST}
         COMMAND $<TARGET_FILE:${TST}>
        )
