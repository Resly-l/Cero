@echo off
setlocal

set "shader_source_path=%~1"
set "output_bin_path=%~2"

if not defined shader_source_path (
    echo Error : No input path provided
    echo Usage : %~nx0 [inputPath] [outputPath]
	exit /b 1
)
if not defined output_bin_path (
    echo Error : No input path provided
    echo Usage : %~nx0 [inputPath] [outputPath]
	exit /b 1
)

set "file=%~dp0config.ini"
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

echo Begin compiling...

for /r %shader_source_path% %%i in (*.vert *.frag) do (
    echo Compiling: "%%i"
    %glslc_path% %shader_source_path%%%~nxi -o %output_bin_path%%%~nxi.spv
)

echo Shader compilation completed.

endlocal