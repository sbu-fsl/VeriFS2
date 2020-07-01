add_executable(fuse-cpp-ramfs main.cpp directory.cpp inode.cpp symlink.cpp file.cpp util.cpp fuse_cpp_ramfs.cpp special_inode.cpp)
set_property(TARGET fuse-cpp-ramfs PROPERTY CXX_STANDARD 11)
target_compile_definitions(fuse-cpp-ramfs PRIVATE FUSE_USE_VERSION=30 _FILE_OFFSET_BITS=64)
if(APPLE)
  target_link_libraries(fuse-cpp-ramfs osxfuse xallocator)
elseif(UNIX) # Linux, BSD etc
  target_link_libraries(fuse-cpp-ramfs fuse xallocator)
endif()
install(TARGETS fuse-cpp-ramfs DESTINATION bin)
