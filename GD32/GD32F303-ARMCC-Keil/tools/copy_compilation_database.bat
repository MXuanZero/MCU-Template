@echo off
set "proj_path=%CD%"
echo %proj_path%
copy /Y %proj_path%\uvision\.vscode\c_cpp_properties.json %proj_path%\.vscode
copy /Y %proj_path%\uvision\.vscode\keil-assistant.log %proj_path%\.vscode
copy /Y %proj_path%\uvision\.vscode\uv4.log %proj_path%\.vscode
echo copy compilation database finish!