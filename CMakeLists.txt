cmake_minimum_required(VERSION 3.18)

project(myDriverFTDI)

# Укажите компиляторы (опционально, если не требуется явно)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Режим сборки
set(CMAKE_BUILD_TYPE Release)

# Путь к библиотеке
link_directories(usr/local/lib/)

# Создание исполняемого файла
add_executable(main_file main.c)

# Установка выходной директории (опционально)
set_target_properties(main_file PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

# Линковка с библиотекой ftd2xx
target_link_libraries(main_file ftd2xx)