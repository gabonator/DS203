@echo off
setlocal enabledelayedexpansion

rem try the drives in reverse order, try not to access cd rom drive, which is mostly the D: drive
set list=G F E D C H I J K L

rem " ONIDSO" - drive label when app running, "DFU V3_10_C" in DFU mode

for %%Z in (%list%) do (
  for /f "tokens=5,*" %%A in ('vol %%Z: 2^> nul') do (
    if not "%%B"=="" (
      set X=%%B
      rem echo drive "%%Z" - label "%%B" - substring "!X:~0,3!"

      if "!X:~0,3!"=="DFU" (
        echo TARGET=%%Z:
        goto :eof
      )
    )
  )
)
