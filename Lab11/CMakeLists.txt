cmake_minimum_required(VERSION 3.0)
project(SemaphoreExample)

set(CMAKE_CXX_STANDARD 17)

add_executable(SemaphoreExample src/main.cpp)

# Add pthread library for thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(SemaphoreExample PRIVATE Threads::Threads)
