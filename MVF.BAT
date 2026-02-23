@echo off

if not defined QDOSDIR goto failed

:startcopy
copy /Y quake\qdos.exe %QDOSDIR%
copy /y quake\qdosfx.exe %QDOSDIR%
copy /Y quake\release\WQDOS.exe %QDOSDIR%
copy /y quake\release_gl\glquake2.exe %QDOSDIR%

copy /Y qw\qw.exe %QDOSDIR%
copy /y qw\qwdosfx.exe %QDOSDIR%
copy /Y qw\release\WQWDOS.exe %QDOSDIR%
copy /y qw\release_gl\glqw2.exe %QDOSDIR%

goto end

:failed
echo QDOSDIR not defined!
pause
goto end

:end
