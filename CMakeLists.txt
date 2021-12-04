cmake_minimum_required(VERSION 3.20)
project(eletjatek C)

set(CMAKE_C_STANDARD 99)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/sdl2)

add_executable(eletjatek {PROJECT_SOURCE_DIR}/ main.c typedefs.h utils.c utils.h game_display.c game_display.h game_logic.c game_logic.h file_handling.c file_handling.h menu.c menu.h menu_element.c menu_element.h menu_text.c menu_text.h game_event_handler.c game_event_handler.h)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Add compiler errors/warnings flags
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

# Add SDL2 library
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Main)

# Add SDL2_image library
#find_package(SDL2_image REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::Image)

# Add SDL2_ttf library
find_package(SDL2_ttf REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::TTF)

# Add SDL2_net library
#find_package(SDL2_net REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::Net)

# Add SDL2_mixer library
#find_package(SDL2_mixer REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::Mixer)

# Add SDL2_gfx library
find_package(SDL2_gfx REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::GFX)