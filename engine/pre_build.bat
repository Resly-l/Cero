@echo off
setlocal

set "batch_path=%~dp0"
set "shader_compiler_path=%batch_path%tool\shader_compiler\compile_shaders.bat"
(
    echo #pragma once
	echo.
    echo #define PATH_ENGINE "%batch_path:\=\\%"
	echo #define PATH_SHADER_COMPILER "%shader_compiler_path:\=\\%"
) > source/io/file/path.generated.h

echo Header file path.generated.h has been created

endlocal