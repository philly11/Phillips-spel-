target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    game.cpp
    player.cpp
    map.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)  # /W4 on MSVC