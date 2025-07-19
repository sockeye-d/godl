inkscape -w 256 -h 256 godl.svg -o godl.png
magick godl.png -define godl:auto-resize="256,128,96,64,48,32,16" godl.ico
rm godl.png
