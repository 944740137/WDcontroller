cmake_minimum_required(VERSION 2.8.3)
project(demo02)
set(CMAKE_BUILD_TYPE Debug)

# set(my_include "/home/wd/lib/libarmadillo/include")
# set(my_lib "/home/wd/lib/libarmadillo/libarmadillo.so")

include_directories(
  ${LAPACK_HEADERS}
  ${ARMADILLO_INCLUDE_DIRS})


add_executable(master 
  src/main.cpp
  src/start.cpp
  src/taskDefine/taskDefine.cpp
  src/processCom/communication.cpp
  src/keyboardIO/keyboardIO.cpp
  src/controller/controller.cpp
  src/wdLog/log.cpp
)
target_include_directories(master SYSTEM PUBLIC
  ${my_include}
  include
  src
  /home/wd/workSpace/WDcontroller/h
)
target_link_libraries(master 
  pthread
  ${PROJECT_SOURCE_DIR}/lib/libjsoncpp.a
)
