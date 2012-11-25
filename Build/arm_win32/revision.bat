@echo off
setlocal enabledelayedexpansion

echo GIT_BUILDPC=%USERDOMAIN%/%USERNAME%

call git --version > nul

if %ERRORLEVEL%==1 (
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

