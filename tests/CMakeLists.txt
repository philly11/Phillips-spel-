set(DOCTEST_WITH_TESTS OFF)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

add_executable(tests
    test_main.cpp
    test_player.cpp
    test_map.cpp
)
target_link_libraries(tests PRIVATE game_core doctest::doctest)
add_test(NAME unit_tests COMMAND tests)