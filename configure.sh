#!/usr/bin/env bash

# get script dir
script_dir=$( cd `dirname ${BASH_SOURCE[0]}` >/dev/null 2>&1 ; pwd -P )

# create build dirs
mkdir -p $script_dir/build/debug_x64
mkdir -p $script_dir/build/release_x64

mkdir -p $script_dir/build/debug_demo_x64
mkdir -p $script_dir/build/release_demo_x64

mkdir -p $script_dir/build/debug_arm64
mkdir -p $script_dir/build/release_arm64

mkdir -p $script_dir/build/debug_demo_arm64
mkdir -p $script_dir/build/release_demo_arm64
