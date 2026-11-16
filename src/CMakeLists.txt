target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)  # /W4 on MSVC

add_library(game_core STATIC
    player.cpp
    map.cpp
    target.cpp

)

target_include_directories(game_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    game.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE game_core)