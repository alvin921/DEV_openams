@echo off


rem @echo %0

@echo %cd%

@for /f %%i in ('cd') do set WORKDIR=%%i


set PJTOOLS=%AMSDIR%\env
set PATH=%PATH%;%PJTOOLS%



cmd
