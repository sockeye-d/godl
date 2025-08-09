
def main [] {
    if (which flatpak-builder | is-empty) {
        print "Couldn't find flatpak-builder. Do you have it installed?"
        exit 1
    }

    let manifest = "./org.fishy.godl.yml" | path expand
    cd /tmp
    sudo flatpak-builder --install flatpak-build/ $manifest --force-clean
}
