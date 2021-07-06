# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project (
"HWRayTracer"
)

# Source
set ( COMMON_ALL
  src/common/global.h 
  src/common/vec3.h 
  src/common/ppm.h
  src/common/ray.h
)

set ( HWRAYTRACER
  ${COMMON_ALL}

  src/HWRayTracer/HWRayTracer.cpp
 
)


add_executable(HWRayTracer  ${HWRAYTRACER})

# 包含子项目。
include_directories(src/common)
