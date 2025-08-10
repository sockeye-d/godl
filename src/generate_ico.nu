#!/usr/bin/nu

def main [] {
    let sizes = [256, 128, 96, 64, 48, 32, 16] | reverse
    mut exit = false

    if ((which inkscape | length) == 0) {
        print "Couldn't find inkscape binary"
        $exit = true
    }

    if ((which magick | length) == 0) {
        print "Couldn't find imagemagick binary"
        $exit = true
    }

    if ((which 7z | length) == 0) {
        print "Couldn't find 7zip binary"
        $exit = true
    }

    if ($exit) {
        exit 1
    }

    $sizes | each {|size|
        print $"Rasterizing ($size)"
        let file = $"godl@($size)x($size).png"
        let _ = inkscape -w $size -h $size godl.svg -o $"godl@($size)x($size).png" | complete
    }
    print "Converting to ICO"
    let _ = magick godl@*x*.png godl.ico | complete

    print "Zipping"
    7z a "godl-icons.zip" godl@*x*.png
    $sizes | each {|size|
        let _ = 7z rn "godl-icons.zip" $"godl@($size)x($size).png" $"($size).png"
    }
    rm --trash godl@*x*.png
    print "Done"
}
