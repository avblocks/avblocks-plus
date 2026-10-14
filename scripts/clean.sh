#!/usr/bin/env bash

# switch to parent directory
script_path=`dirname ${BASH_SOURCE[0]}`
pushd $script_path/..

while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        # build type  
        # one of: "debug", "release", "debug_demo", "release_demo"
        -t|--type)
            type="$2"
            shift # past argument
            shift # past value
            ;;
        # platform: "x64" or "arm64". Defaults to the host architecture.
        -p|--platform)
            platform="$2"
            shift # past argument
            shift # past value
            ;;
        *)    # unknown option
            shift # past argument
            ;;
    esac
done

echo "Running clean.sh ..."

if [[ -z $type ]]; then
    echo "Usage:"    
    echo './build.sh --type [debug, release, debug_demo, release_demo] [--platform x64|arm64]'
    popd; exit 1
fi

declare -A supported_types=([debug]=1 [release]=1 [debug_demo]=1 [release_demo]=1)

if [[ -z "${supported_types[$type]}" ]]; then
    echo "Usage:"    
    echo './build.sh --type [debug, release, debug_demo, release_demo] [--platform x64|arm64]'
    popd; exit 1
fi

if [[ -z $platform ]]; then
    if [[ "$(uname -m)" == "arm64" || "$(uname -m)" == "aarch64" ]]; then
        platform=arm64
    else
        platform=x64
    fi
fi

echo "type: $type"
echo "platform: $platform"

build_dir=./build/${type}_${platform}
mkdir -p $build_dir
pushd $build_dir
    ninja clean
    ret=$?
popd


popd; exit $ret