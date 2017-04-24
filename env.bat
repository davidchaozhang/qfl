set runpath=.\workspace\x64

set cv_ver=310
set cvpath=C:\pkg\opencv300\win7-vc2013-64\install
set zbarpath=..\3rdparty
set boostpath=C:\pkg\boost_1_56_0
set cudapath=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v7.5
set path=%cvpath%\x64\vc12\bin\;%cudapath%\bin;%runpath%\debug;%runpath%\release;%boostpath%\lib64-msvc-12.0;%zbarpath%\lib;%path%

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe" ./workspace/qfl.sln

pause