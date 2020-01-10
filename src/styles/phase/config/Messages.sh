#! /bin/sh
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/kstyle_phase_config.pot
