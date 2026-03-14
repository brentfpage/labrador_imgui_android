cmake_minimum_required(VERSION 3.16)
project(librador VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(ENV{JAVA_HOME} "/Library/Java/JavaVirtualMachines/jdk-25.jdk/Contents/Home")

add_library(librador SHARED
    librador.cpp librador.h
    librador_global.h
    librador_internal.h
    logging.h
    logging_internal.h
    o1buffer.cpp o1buffer.h
    usbcallhandler.cpp usbcallhandler.h
)

target_include_directories(librador PUBLIC librador_deps/libusb)
target_include_directories(librador PUBLIC librador_deps/libdfuprog)
# target_include_directories(librador PUBLIC /Library/Java/JavaVirtualMachines/jdk-25.jdk/Contents/Home/include)

add_library(libusb SHARED IMPORTED)
set_target_properties(libusb PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/librador_deps/libusb/android/${ANDROID_ABI}/libusb1.0.so)

add_library(libdfuprog SHARED IMPORTED)
set_target_properties(libdfuprog PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/librador_deps/libdfuprog/android/${ANDROID_ABI}/libdfuprog-0.9.so)

# add_library(libjvm SHARED IMPORTED)
# set_target_properties(libjvm PROPERTIES IMPORTED_LOCATION /Library/Java/JavaVirtualMachines/jdk-25.jdk/Contents/Home/lib/server/libjvm.so)

# https://github.com/android/ndk/issues/1905
file(READ "${CMAKE_ANDROID_NDK}/meta/abis.json" JSON_FILE)
string(JSON TRIPLE GET "${JSON_FILE}" "${ANDROID_ABI}" "triple")

add_library(libnativehelper SHARED IMPORTED)
set_target_properties(libnativehelper PROPERTIES IMPORTED_LOCATION ${ANDROID_NDK}/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/lib/${TRIPLE}/35/libnativehelper.so) # possibly necessary to substitute ${ANDROID_NATIVE_API_LEVEL} for 35?

target_link_libraries(librador libusb libdfuprog libnativehelper)
