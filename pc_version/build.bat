@echo off
REM simple build script, out of laziness but this does it for my needs

REM get developer command prompt context
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"

REM in my case debug simply means to print the instrumentation logs
IF "%2" == "log" (SET log="/DSHOULD_LOG") ELSE (SET log=)

SET include_dir=deps/include
SET include_dir2=../include
SET lib_dir=deps/lib

set libs=sfml-main.lib sfml-graphics.lib sfml-system.lib sfml-window.lib

SET build_folder=build
SET out_file=%build_folder%/game.exe

IF NOT EXIST "%build_folder%" MKDIR %build_folder%

SET src= main.cpp ^
graphics_pc.cpp ^
input_manager_pc.cpp ^
audio_pc.cpp ^
../src/level.cpp ^
../src/main_game.cpp ^
../src/player.cpp ^
../src/enemy.cpp ^
../src/status_bar.cpp

REM call VC++ compiler
IF "%1" == "debug" (
    cl %log% /I "%include_dir%" /I "%include_dir2%" /EHsc /Zi /Fd"%build_folder%/" /MT %src% /Fo"%build_folder%/" /link /DEBUG /LIBPATH:"%lib_dir%" %libs% /out:%out_file%
) ELSE (
    cl %log% /I "%include_dir%" /I "%include_dir2%" /EHsc /MT %src% /Fo"%build_folder%/" /link /LIBPATH:"%lib_dir%" %libs% /out:%out_file%
)

IF errorlevel 1 (
    ECHO compilation failed
) ELSE (
    REM run the resulting game
    START %out_file%
)
