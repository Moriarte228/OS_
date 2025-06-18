cmake_minimum_required(VERSION 3.15)

project(EmployeeRecordSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(HEADERS
        Employee.h
        MessageProtocol.h
        CriticalSectionGuard.h
        RecordLock.h
        FileController.h
        ServerController.h
)

set(SOURCES_COMMON
        RecordLock.cpp
        FileController.cpp
        ServerController.cpp
)

add_executable(Server
        Server.cpp
        ${SOURCES_COMMON}
        ${HEADERS}
)

add_executable(Client
        Client.cpp
        ${HEADERS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(Server Threads::Threads)
target_link_libraries(Client Threads::Threads)
