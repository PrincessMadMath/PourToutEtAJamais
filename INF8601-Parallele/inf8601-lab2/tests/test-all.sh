#!/bin/sh

${abs_top_srcdir}/encode/encode --cmd check
RET=$?
exit $RET
