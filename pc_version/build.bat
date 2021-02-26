REM simple build script, out of laziness but this does it for my needs

REM get developer command prompt context
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"

REM in my case debug simply means to print the instrumentation logs
IF "%1" == "debug" (SET debug="/DDEBUG") ELSE (SET debug=)

SET build_folder=build
SET out_file=%build_folder%/game.exe

IF NOT EXIST "%build_folder%" MKDIR %build_folder%

REM call VC++ compiler, dummy file names for the time being
cl %debug% /EHsc main.cpp ../src/main_game.cpp /Fo"%build_folder%/" /link /out:%out_file%

REM run the resulting game
START %out_file%
