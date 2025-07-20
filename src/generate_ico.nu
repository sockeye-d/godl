#!/usr/bin/nu

def main [] {
    mut exit = false

    if ((which inkscape | length) == 0) {
        print "Couldn't find inkscape binary"
        $exit = true
    }

    if ((which magick | length) == 0) {
        print "Couldn't find inkscape binary"
        $exit = true
    }

    if ($exit) {
        exit 1
    }

    print "Rasterizing SVG"
    let _ = inkscape -w 256 -h 256 godl.svg -o godl.png | complete
    print "Converting to ICO"
    let _ = magick godl.png -define godl:auto-resize="256,128,96,64,48,32,16" godl.ico | complete
    print "Removing temporaries"
    rm godl.png
    print "Done"
}
