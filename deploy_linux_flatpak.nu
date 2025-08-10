def main [--clean, --disable-cache] {
    if (which flatpak-builder | is-empty) {
        print "Couldn't find flatpak-builder. Do you have it installed?"
        exit 1
    }

    let manifest = "./io.github.sockeye_d.godl.yml" | path expand
    cd /tmp
    if $clean {
        print "Cleaning"
        rm -r godl-flatpak-build/
    }
    if $disable_cache {
        print "Disabling cache"
        sudo flatpak-builder --install godl-flatpak-build/ $manifest --force-clean --disable-cache
    } else {
        sudo flatpak-builder --install godl-flatpak-build/ $manifest --force-clean
    }
}
