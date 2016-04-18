@perl -v
@@IF ERRORLEVEL 1  GOTO L_no_perl

@CD ..\..\..\..\core\src\dvr\net
@perl bin\cfg-compile.pl -o common/inc/auto_syscfg.h common/conf/amn_system.ucos.conf
@ECHO "Check the auto_syscfg.h is generated automatically"
@DIR common\inc\auto_syscfg.h

REM @mkdir http\chp_C
REM @perl bin\chp-compile.pl -o http/chp_C/ http/chp/
REM @ECHO "Check the CHP files are generated automatically"
REM @DIR http\chp_C

@GOTO End


:L_no_perl
@ECHO "No PERL executable exists in your PATH, please install it first"
@ECHO "You may download from here: http://www.activestate.com/activeperl/downloads"

GOTO End


:End
@PAUSE
