// HWRayTracer.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>

#include "global.h"
#include "Sphere.cpp"
#include "Sence.cpp"
#include "Camera.h"
#include "Materail.h"

SYSTEM_INFO sysInfo;

int image_width;
int image_height;
Camera camera;
int antialiasing_sample_nums;
int ray_trace_maxdepth;
Sence sence;
vector<string> out_ppm_list;
int remains_lines;

int RenderThread(int start_height, int end_height, int thread_id);
void Showprogress();

// TODO: 在此处引用程序需要的其他标头。
