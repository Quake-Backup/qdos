@echo off
set QDOSBINDIR=Debug
SET SEVENZIPPATH=E:\Program Files\7-zip

if not defined QDOSDEVBASE ( goto failed )

cd /d "%QDOSDEVBASE%\quake\"
call clean.bat
cd /d "%QDOSDEVBASE%\qw\"
call clean.bat
call vcvars32.bat
call "%VS80COMNTOOLS%vsvars32.bat"

:startcopy
cd /d "%QDOSDEVBASE%\quake\"
devenv "%QDOSDEVBASE%\quake\winquake.sln" /Build "%QDOSBINDIR%|Win32" /Project "winquake"
devenv "%QDOSDEVBASE%\quake\winquake.sln" /Build "GL %QDOSBINDIR%|Win32" /Project "winquake"
copy /y "%QDOSDEVBASE%\quake\%QDOSBINDIR%\WQDOS.exe" "%QDOSDEVBASE%\"
copy /y "%QDOSDEVBASE%\quake\%QDOSBINDIR%_gl\glquake2.exe" "%QDOSDEVBASE%\"

cd /d "%QDOSDEVBASE%\qw\"
devenv "%QDOSDEVBASE%\qw\qwcl.sln" /Build "%QDOSBINDIR%|Win32" /Project "qwcl"
devenv "%QDOSDEVBASE%\qw\qwcl.sln" /Build "GL QW %QDOSBINDIR%|Win32" /Project "qwcl"
copy /y "%QDOSDEVBASE%\qw\%QDOSBINDIR%\WQWDOS.EXE" "%QDOSDEVBASE%\"
copy /y "%QDOSDEVBASE%\qw\%QDOSBINDIR%_gl\glqw2.exe" "%QDOSDEVBASE%\"

cd /d "%QDOSDEVBASE%"
call mvd.bat

:buildzip
del /q "%QDOSDEVBASE%\DWQDOS_EXE_LATEST.7Z"
del /q "%QDOSDEVBASE%\DWQWDOS_EXE_LATEST.7Z"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\WQDOS.exe"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\glquake2.exe"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\quake\changes.txt"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\quake\README.QD"

"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQWDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\WQWDOS.exe"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQWDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\glqw2.exe"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQWDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\qw\changes.txt"
"%SEVENZIPPATH%\7z.exe" a -t7z -mx9 -mmt "%QDOSDEVBASE%\DWQWDOS_EXE_LATEST.7Z" "%QDOSDEVBASE%\qw\README.QWD"

goto end

:failed
echo QDOSDEVBASE not defined!
pause
goto end

:end
set QDOSBINDIR=
