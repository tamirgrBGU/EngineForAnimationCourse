#!/bin/bash
currnt_dir=`dirname "$0"`
echo Enter dir name
read name
mkdir -p $currnt_dir/tutorial/$name
echo "" >> $currnt_dir/tutorial/CMakeLists.txt
echo "add_subdirectory("$name")" >> $currnt_dir/tutorial/CMakeLists.txt

sed "s/Demo/$name/gi" $currnt_dir/tutorial/Demo/CMakeLists.txt > $currnt_dir/tutorial/$name/CMakeLists.txt
sed "s/Demo/$name/gi" $currnt_dir/tutorial/Demo/Demo.cpp > $currnt_dir/tutorial/$name/$name.cpp
sed "s/Demo/$name/gi" $currnt_dir/tutorial/Demo/BasicScene.h > $currnt_dir/tutorial/$name/BasicScene.h
sed "s/Demo/$name/gi" $currnt_dir/tutorial/Demo/BasicScene.cpp > $currnt_dir/tutorial/$name/BasicScene.cpp
