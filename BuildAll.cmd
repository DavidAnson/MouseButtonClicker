call :BUILD x86
call :BUILD x64
goto :EOF

:BUILD
mkdir %1
del %1\MouseButtonClicker.*
"%WindowsSdkVerBinPath%%1\rc" /fo %1\MouseButtonClicker.res MouseButtonClicker.rc
"%VCToolsInstallDir%bin\Hostx86\%1\cl" MouseButtonClicker.cpp %1\MouseButtonClicker.res /O1 /sdl /W4 /utf-8 /Fo%1\MouseButtonClicker.obj /Fe%1\MouseButtonClicker.exe /link user32.lib /LIBPATH:"%VCToolsInstallDir%lib\%1" /LIBPATH:"%WindowsSdkDir%Lib\%WindowsSDKLibVersion%ucrt\%1" /LIBPATH:"%WindowsSdkDir%Lib\%WindowsSDKLibVersion%um\%1" /NOCOFFGRPINFO
goto :EOF
