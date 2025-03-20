cmake_minimum_required(VERSION 3.30)
project(StudentDBMS C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(ESPEAK espeak)  # Changed to optional

# Include directories
include_directories(
        ${GTK4_INCLUDE_DIRS}
        ${SQLITE3_INCLUDE_DIRS}
        ${CAIRO_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src/db
        ${CMAKE_SOURCE_DIR}/src/gui
        ${CMAKE_SOURCE_DIR}/src/visuals
        ${CMAKE_SOURCE_DIR}/src/utils
)

# Conditionally include espeak if found
if(ESPEAK_FOUND)
    include_directories(${ESPEAK_INCLUDE_DIRS})
    link_directories(${ESPEAK_LIBRARY_DIRS})
    add_definitions(-DHAVE_ESPEAK)  # Define a macro to enable espeak in code
endif()

# Link directories
link_directories(
        ${GTK4_LIBRARY_DIRS}
        ${SQLITE3_LIBRARY_DIRS}
        ${CAIRO_LIBRARY_DIRS}
)

# Define source files
set(SOURCES
        main.c
        src/db/db.c
        src/gui/main_window.c
        src/gui/form_view.c
        src/gui/list_view.c
        src/visuals/charts.c
        src/utils/voice.c
        src/utils/themes.c
        src/utils/logs.c
        src/utils/gui_utils.c  # Added
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
        ${GTK4_LIBRARIES}
        ${SQLITE3_LIBRARIES}
        ${CAIRO_LIBRARIES}
)

# Conditionally link espeak if found
if(ESPEAK_FOUND)
    target_link_libraries(${PROJECT_NAME} ${ESPEAK_LIBRARIES})
endif()

# Set output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Copy resources and data to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)
file(COPY ${CMAKE_SOURCE_DIR}/data/ DESTINATION ${CMAKE_BINARY_DIR}/data/)
