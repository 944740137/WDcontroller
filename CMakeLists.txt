cmake_minimum_required(VERSION 2.8.3)
project(controller)
set(CMAKE_BUILD_TYPE Debug)

# 1、find_package（批量引入库文件和头文件）

# 2、include_directories（引入头文件目录）
# 3、link_directories（引入库文件目录到当前工程）

# 4、link_libraries (引入库文件到当前工程)
# 5、target_link_libraries（引入库文件到子工程）
# 6、target_include_directories（引入头文件目录到子工程）

link_directories(
  ${PROJECT_SOURCE_DIR}/lib
  )

add_executable(master 
  src/start/main.cpp
  src/start/start.cpp
  src/taskDefine/taskDefine.cpp
  src/processCom/communication.cpp
  src/keyboardIO/keyboardIO.cpp
  src/controller/controller.cpp
  src/robot/robot.cpp
  src/wdLog/log.cpp
  src/event/signalEvent.cpp
)
target_include_directories(master SYSTEM PUBLIC
  include
  src
  ${PROJECT_SOURCE_DIR}/../h
)
target_link_libraries(master 
  pthread
  jsoncpp
  event
)
