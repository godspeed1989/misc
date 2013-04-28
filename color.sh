#!/bin/sh

bkg_color=45  #背景颜色 40~47
font_color=36 #字体颜色 30~37

color_output () {
	echo -n "\033[$font_color;$bkg_color;1m"
	echo -n $*
	echo -n "\033[0m"
}

str="Hello World"
color_output $str
echo ""

line=15
col=40

i=0
while [ "$i" -le "$line" ]
do
	j=0
	while [ "$j" -le "$col" ]
	do
		color_output "-"
		j=$(($j+1))
	done
	echo ""
	i=$(($i+1))
done

