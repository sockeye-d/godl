#!/usr/bin/nu

def main [--clean] {
    let sizes = [512, 256, 128, 96, 64, 48, 32, 16] | reverse
    mut should_exit = false

    if $clean {
        rm godl.ico -f
        rm -rf icon-raster/
        exit 0
    }

    if ((which inkscape | length) == 0) {
        print "Couldn't find inkscape binary"
        $should_exit = true
    }

    if ((which magick | length) == 0) {
        print "Couldn't find imagemagick binary"
        $should_exit = true
    }

    if ((which 7z | length) == 0) {
        print "Couldn't find 7zip binary"
        $should_exit = true
    }

    if ($should_exit) {
        exit 1
    }

    mkdir icon-raster
    $sizes | each {|size|
        print $"Rasterizing ($size)"
        let file = $"godl@($size)x($size).png"
        inkscape -w $size -h $size godl.svg -o $"icon-raster/($size).png" e> /dev/null
    }
    print "Converting to ICO"
    let _ = magick icon-raster/*.png godl.ico | complete
    print "Done"
}
