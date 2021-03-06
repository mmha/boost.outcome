#!/bin/bash
rm -rf send_to_wandbox_tmp
mkdir send_to_wandbox_tmp
include/boost/monad/bindlib/scripts/GenSingleHeader.py -DBOOST_OUTCOME_DISABLE_VALGRIND=1 -Evalgrind include/boost/monad/future.hpp > include/boost/monad/single_include.hpp
sed "1s/.*/#include \"single_include.hpp\"/" example/monad_example.cpp > send_to_wandbox.cpp
gcc -fpreprocessed -dD -E -P include/boost/monad/single_include.hpp > send_to_wandbox_tmp/single_include2.hpp 2>/dev/null
sed "/^$/d" send_to_wandbox_tmp/single_include2.hpp > send_to_wandbox_tmp/single_include.hpp
rm -rf send_to_wandbox_tmp/single_include2.hpp
URL=`include/boost/monad/bindlib/scripts/send_to_wandbox.py send_to_wandbox_tmp send_to_wandbox.cpp | sed -e 's/.*\(http:\/\/[^ '"'"']*\).*/\1/'`
if [[ $FRAME != "" ]]; then
    echo '<iframe src="'$URL'" frameborder="0" style="height: 100%; width: 100%;" height="100%" width="100%"></iframe>'
else
    echo $URL
fi
