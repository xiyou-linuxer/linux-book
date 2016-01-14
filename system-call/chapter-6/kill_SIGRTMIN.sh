#!/bin/bash

pid=$(ps -aux|grep realiable_signal | grep -v grep |awk '{print $2}')

for((i=0;i<5;i++))
do
	kill -n 34 $pid

done

