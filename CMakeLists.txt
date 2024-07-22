cmake_minimum_required(VERSION 3.10)

# 项目名称
project(aikit_project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/workspace)

# 查找所需的包
find_package(nlohmann_json 3.11.3 REQUIRED PATHS ~/project/rtsp_opencv_gpu_server/lib/install_json)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(oatpp REQUIRED) # 添加这一行以查找 oatpp 库

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/lib/tts_libs/include
    ${PROJECT_SOURCE_DIR}/lib/install_json/include
    ${PROJECT_SOURCE_DIR}/src/
    ${PROJECT_SOURCE_DIR}/lib/oatpp_install/include/oatpp-1.3.0/oatpp
)

# 链接库目录
link_directories(
    ${PROJECT_SOURCE_DIR}/lib/tts_libs/lib
    ${PROJECT_SOURCE_DIR}/lib/install_obb/lib
    ${PROJECT_SOURCE_DIR}/lib/oatpp_install/lib/oatpp-1.3.0
)

# 添加库文件
add_library(${PROJECT_NAME}_lib
    src/controller/MyController.cpp
    src/xtts/XTTSClient.cpp
)

# 添加可执行文件
add_executable(xtts_server 
    src/App.cpp
)

# 链接库文件
target_link_libraries(
    xtts_server 
    ${PROJECT_NAME}_lib
    aikit
    nlohmann_json::nlohmann_json
    oatpp::oatpp # 添加这一行以链接 oatpp 库
    Threads::Threads 
    spdlog::spdlog
)

# 设置运行时库路径
set_target_properties(xtts_server PROPERTIES
    BUILD_RPATH ${PROJECT_SOURCE_DIR}/lib/tts_libs/lib
)

# 自定义命令来复制依赖库到输出目录
if(EXISTS "${PROJECT_SOURCE_DIR}/lib/tts_libs/lib")
    add_custom_command(TARGET xtts_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/lib/tts_libs/lib $<TARGET_FILE_DIR:xtts_server>
    )
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/lib/install_obb/lib")
    add_custom_command(TARGET xtts_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/lib/install_obb/lib $<TARGET_FILE_DIR:xtts_server>
    )
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/lib/oatpp_install/lib/oatpp-1.3.0")
    add_custom_command(TARGET xtts_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/lib/oatpp_install/lib/oatpp-1.3.0 $<TARGET_FILE_DIR:xtts_server>
    )
endif()
