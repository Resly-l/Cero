@echo off
setlocal enabledelayedexpansion

set "file=config.ini"
set "shader_source_path="
set "outoput_bin_path="

set "glslc_path="
for /f "tokens=1,* delims==" %%a in (%file%) do (
    if /i "%%a"=="glslc_path" (
        set "glslc_path=%%b"
    )
)

if not defined glslc_path (
    echo glslc path not found in config.ini.
    exit /b 1
)

for /f "tokens=1,* delims==" %%a in (%file%) do (
    if /i "%%a"=="shader_source_path" (
        set "shader_source_path=%%b"
    )
)

if not defined shader_source_path (
    echo Shader source path not found in config.ini.
    exit /b 1
)

for /f "tokens=1,* delims==" %%a in (%file%) do (
    if /i "%%a"=="output_bin_path" (
        set "output_bin_path=%%b"
    )
)

if not defined output_bin_path (
    echo Shader output path not found in config.ini.
    exit /b 1
)

for /r %shader_source_path% %%i in (*.vert *.frag) do (
    echo Compiling: "%%i"
    %glslc_path% %shader_source_path%%%~nxi -o %output_bin_path%%%~nxi.spv
)

echo Shader compilation completed.

endlocal