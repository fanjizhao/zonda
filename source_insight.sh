#!/bin/bash
find . -name "*.c" > source_insight.tmp
find . -name "*.cpp" >> source_insight.tmp
find . -name "*.h" >> source_insight.tmp
grep -v ^\./vendor/ source_insight.tmp | grep -v ^\./config\.h$ | sed '1,$s/^\./y\:/g' | sed '1,$s/\//\\/g' > source_insight.list
