@echo ----------------IMPORTANT NOTICE-----------------------
@echo Make sure that your environment variable PATH contains
@echo binary executable dir of PYTHON, RUBY and PERL, or 
@echo the compile may FAILURE!
@echo -------------------------------------------------------
@call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86
@set PATH=%cd%\src\qt\3rdparty\gnuwin32\bin;%PATH%
@echo %PATH%
@pause
@call python build.py -d -c --skip-git