@echo off
setlocal enabledelayedexpansion

rem Todo: if GIT_BUILDPC contain spaces, the compilation will fail!
echo GIT_BUILDPC=%USERDOMAIN%/%USERNAME%
      
where /Q git

if %ERRORLEVEL%==1 (
  rem Git not installed
  goto :eof
)

for /F %%i in ('git shortlog -s') do (
  set /A acc=!acc!+%%i
)
for /F %%i in ('git rev-parse --short HEAD') do (
  set hash=%%i
)
echo GIT_REVNUMBER=!acc!
echo GIT_REVHASH=!hash!

