set runpath=.\workspace\x64

set cv_ver=330
set cvpath=C:\pkg\opencv3.3.0\build_vs14\install
set zbarpath=..\3rdparty
set path=%cvpath%\x64\vc14\bin\;%runpath%\debug;%runpath%\release;%zbarpath%\lib;%path%

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" ./workspace/qfl.sln

pause