@echo off

del Debug\*.* /q /f
del Release\*.* /q /f
rmdir /q Debug
rmdir /q Release
del *.user *.ncb *.suo

goto :eof
del /s /a:r *.suo 
del /s *.ncb 
del /s *.pch 
del /s *.pdb 
del /s *.bsc 
del /s *.idb 
del /s *.sbr 
del /s *.ilk 
del /s *.map 
del /s *.aps *.opt 
del /s *.plg 
del /s *.user

del /q ds203ui\Debug\*.*
del /q ds203ui\Release\*.*
