param(
    # build type
    # one of: "debug", "release", "debug_demo", "release_demo"
    [String]$type=""
)

# switch to parent directory	
pushd "$PSScriptRoot/.."

echo "Running clean.ps1 ..."

if (! $type) {
    echo "Usage:"    
    echo '.\clean.ps1 -type [debug, release, debug_demo, release_demo]'
    popd; exit 1
}

if (! @("debug", "release", "debug_demo", "release_demo").Contains($type.ToLower())) {
    echo "Usage:"    
    echo '.\clean.ps1 -type [debug, release, debug_demo, release_demo]'
    popd; exit 1
}

echo "type = $type"

$build_dir = "./build/${type}_x64"

if (Test-Path $build_dir) {
    pushd $build_dir
        ninja clean
        $ret = $LASTEXITCODE
    popd
} else {
    echo "Build directory does not exist: $build_dir"
    $ret = 0
}

popd
exit $ret
