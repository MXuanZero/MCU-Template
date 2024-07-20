@echo off
set build_dir="build"

if not exist %build_dir% (
	mkdir %build_dir%
)
cmake -B %build_dir% -S . -G Ninja
cmake --build %build_dir% -- -j16