add_library(game_core STATIC
    ability.cpp
    player.cpp
    map.cpp
    target.cpp
    combat.cpp
    weapon.cpp
    spread.cpp
    pickup.cpp
    wave.cpp
)
target_include_directories(game_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(game_core PRIVATE -Wall -Wextra)  # /W4 on MSVC

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    game.cpp
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_link_libraries(${PROJECT_NAME} PRIVATE game_core)