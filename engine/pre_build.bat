@echo off
setlocal

set "engine_path=%~dp0"
set "engine_asset_path=%engine_path%asset\"
set "shader_compiler_path=%engine_path%tool\shader_compiler\compile_shaders.bat"
(
    echo #pragma once
	echo.
    echo #define PATH_ENGINE "%engine_path:\=/%"
    echo #define PATH_ENGINE_ASSET "%engine_asset_path:\=/%"
	echo #define PATH_SHADER_COMPILER "%shader_compiler_path:\=/%"
) > source/file/path.generated.h

echo Header file path.generated.h has been created

endlocal