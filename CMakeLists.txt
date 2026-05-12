cmake_minimum_required(VERSION 3.30)
include(cmake/ArmToolchain.cmake)

project(dual-elevator-system C)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
enable_language(ASM)

set(PROJECT_PATH                         ${CMAKE_CURRENT_SOURCE_DIR})
include(cmake/Device.cmake)
include(cmake/SourceList.cmake)
include(cmake/IncludeList.cmake)

set(LINKER_SCRIPTS_BASE_DIR              ${PROJECT_PATH}/STM32-base/linker/)
set(STARTUP_BASE_DIR                     ${PROJECT_PATH}/STM32-base/startup/)

# Remove any main*.c files that SourceList.cmake might have automatically picked up.
# This prevents "multiple definition of main" errors during compilation.
list(FILTER SOURCE_LIST EXCLUDE REGEX "main.*\\.c$")

# Define the two distinct executables you want to build
set(ELEVATOR_TARGETS main_master main_slave)

# Loop through both targets and apply the exact same build rules to them
foreach(TARGET_NAME IN LISTS ELEVATOR_TARGETS)
    
    # 1. Create the executable linking its specific main file and the shared libraries
    add_executable(${TARGET_NAME} 
        src/${TARGET_NAME}.c 
        ${SOURCE_LIST} 
        ${STARTUP_BASE_DIR}/${SERIES_FOLDER}/${MAPPED_DEVICE}.s
    )

    # 2. Include Directories
    target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDE_LIST})
    
    # 3. Project Symbols
    target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILER_DEFINE})
    
    # 4. Compiler Options
    target_compile_options(${TARGET_NAME} PUBLIC
            ${CPU_PARAMETERS}
            -Wall
            -Wpedantic
            -Wno-unused-parameter
    )

    # 5. Linker Options
    target_link_options(${TARGET_NAME} PRIVATE
            ${CPU_PARAMETERS}
            -L${LINKER_SCRIPTS_BASE_DIR}
            -T${LINKER_SCRIPTS_BASE_DIR}/${SERIES_FOLDER}/${DEVICE}.ld
            -Xlinker --gc-sections
            -Wl,-Map=${TARGET_NAME}.map
            --specs=nano.specs
            --specs=nosys.specs
    )

    # 6. Execute post-build to print size
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
    )
    
    # 7. Convert output to .hex and .bin
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.hex
    )

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.bin
    )

endforeach()