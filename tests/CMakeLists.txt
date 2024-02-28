set(USTREAM_UNIT_TESTS ustream_tests)

include(CTest)

file(GLOB TARGET_SRC "./tests.cpp" )

add_executable(${USTREAM_UNIT_TESTS} ${TARGET_SRC})

add_test(${USTREAM_UNIT_TESTS} ${USTREAM_UNIT_TESTS})