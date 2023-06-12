#!/bin/bash

rm vert.spv
rm frag.spv

glslc src/drawing/render/shaders/shader.vert -o vert.spv
glslc src/drawing/render/shaders/shader.frag -o frag.spv
