##
## EPITECH PROJECT, 2026
## Project - Zappy
## File description:
## GUI unit tests (Criterion)
##

list(APPEND CMAKE_PREFIX_PATH /opt/homebrew)

find_path(CRITERION_INCLUDE_DIRS criterion/criterion.h REQUIRED)
find_library(CRITERION_LIBRARIES criterion REQUIRED)

set(GUI_SRC ${CMAKE_SOURCE_DIR}/gui/src)

add_executable(zappy_gui_tests
    gui/test_gui_args.cpp
    gui/test_player.cpp
    gui/test_response_parser.cpp
    gui/test_pack_manifest.cpp
    gui/test_team_colors.cpp
    ${GUI_SRC}/GuiArgs.cpp
    ${GUI_SRC}/Game/Player.cpp
    ${GUI_SRC}/Game/Entity.cpp
    ${GUI_SRC}/Game/GameState.cpp
    ${GUI_SRC}/game/Resources/Resources.cpp
    ${GUI_SRC}/networking/ResponseParser/ResponseParser.cpp
    ${GUI_SRC}/Theme/PackManifest.cpp
)

target_include_directories(zappy_gui_tests
    PRIVATE ${CMAKE_SOURCE_DIR}/gui/include
    PRIVATE ${CMAKE_SOURCE_DIR}/gui/src
    PRIVATE ${CRITERION_INCLUDE_DIRS}
)

target_link_libraries(zappy_gui_tests
    PRIVATE ${CRITERION_LIBRARIES}
)

target_compile_definitions(zappy_gui_tests
    PRIVATE PACKS_DIR="${CMAKE_SOURCE_DIR}/gui/packs/"
    PRIVATE ASSETS_DIR="${CMAKE_SOURCE_DIR}/gui/assets/"
)

enable_testing()
add_test(NAME gui COMMAND zappy_gui_tests)
