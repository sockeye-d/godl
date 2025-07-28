
def main [release_cycle: string = "stable", zip_dir: string = "deploy"] {
    if (which 7z | is-empty) {
        print "Couldn't find 7-zip"
        exit 1
    }

    let current_dir = pwd

    let build_dir: string = ls build/* | sort-by modified | last | get name | path expand
    print $"Assuming build directory is ($build_dir)"

    if ((input -d Y "Is this okay? [Y/n] " | str downcase) == "n") {
        exit 1
    }

    let deploy_dir = $"($build_dir)\\bin"
    print $"Zipping files in ($deploy_dir)"

    let zip_name = $zip_dir | path join $"godl-(git describe --tags --abbrev=0)-($release_cycle)-win64.zip"

    print $zip_name

    let files = glob ($zip_dir | path join $"($deploy_dir | str replace --all '\' '/')/*")

    7z a $zip_name ...$files

    if ((input -d N "Upload to GitHub? [y/N] " | str downcase) == "y") {
        if (which 7z | is-empty) {
            print "Couldn't find gh CLI"
            exit 1
        }
        gh release upload (git describe --tags --abbrev=0) $zip_name

        if ((input -d N "Remove zip file? [y/N] " | str downcase) == "y") {
            rm $zip_name
        }
    }
}
