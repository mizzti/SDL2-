#标题
cmake_minimum_required(VERSION 3.10.0)
project(SDL_shooter VERSION 0.1.0 LANGUAGES C CXX)#第一个就是项目名称

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置编译选项
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic)# mac/unix
endif()

#设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})#将可执行文件输出到CmakeList所在目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#设置输出可执行文件名称
set(TARGET ${PROJECT_NAME}_${CMAKE_SYSTEM_NAME})

#查找并载入CMake预设
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

#添加可执行文件
add_executable(${TARGET}
                "src/main.cpp"
                "src/Game.cpp"
                "src/SceneTitle.cpp"
                "src/Scene.cpp"
                "src/SceneEnd.cpp"
                "src/SceneMain.cpp") #可执行文件即.cpp文件，需要按照其所在目录添加

#链接库
target_link_libraries(${TARGET} 
                        ${SDL2_LIBRARIES}
                        SDL2_image::SDL2_image
                        SDL2_mixer::SDL2_mixer
                        SDL2_ttf::SDL2_ttf)