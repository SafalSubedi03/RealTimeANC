@echo off
REM --------------------------------------
REM Build Real-Time Audio DSP Engine (Windows / MSVC x64)
REM Includes PortAudio ring buffer (pa_ringbuffer.c)
REM --------------------------------------

REM Initialize Visual Studio x64 environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM ---- Paths ----
set SRC_DIR=src
set INCLUDE_DIR=include
set PORTAUDIO_INC=external\portaudio\include
set PORTAUDIO_LIB=external\portaudio\lib
set PORTAUDIO_BIN=external\portaudio\bin
set BUILD_DIR=build

REM ---- Create build folder if it doesn't exist ----
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM ---- Compile all C++ source files ----
echo Compiling C++ source files...
for %%f in (%SRC_DIR%\*.cpp) do (
    echo Compiling %%f...
    cl.exe /c /EHsc /std:c++20 ^
        /I %INCLUDE_DIR% ^
        /I %PORTAUDIO_INC% ^
        "%%f" ^
        /Fo%BUILD_DIR%\%%~nf.obj
)

REM ---- Compile PortAudio ring buffer (C source) ----
echo Compiling pa_ringbuffer.c...
cl.exe /c /TC ^
    /I %PORTAUDIO_INC% ^
    %SRC_DIR%\pa_ringbuffer.c ^
    /Fo%BUILD_DIR%\pa_ringbuffer.obj

REM ---- Link all object files ----
echo Linking...
cl.exe %BUILD_DIR%\*.obj ^
    /link ^
    /LIBPATH:%PORTAUDIO_LIB% ^
    portaudio_x64.lib winmm.lib ole32.lib user32.lib ^
    /OUT:%BUILD_DIR%\audio_dsp.exe

REM ---- Copy PortAudio DLL ----
echo Copying PortAudio DLL...
copy /Y %PORTAUDIO_BIN%\portaudio_x64.dll %BUILD_DIR%\portaudio_x64.dll

echo.
echo Build Complete!
echo Executable and DLL are in "%BUILD_DIR%"
pause
