# Download AVBlocks Core and Assets on Linux

Change to the directory where you cloned this repository:

```bash
cd avblocks-plus
```

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

Builds are available for ARM64 (`linux-arm64`) and x64 (`linux`). Use `uname -m` to check your machine: `aarch64` means ARM64, `x86_64` means x64.

```bash
# select version and platform
# "linux-arm64" for ARM64 (aarch64), "linux" for x64
tag="v3.4.0-demo.1"
platform="linux"

# download
mkdir -p ./sdk
cd ./sdk

# sdk
curl \
  --location \
  --output ./avblocks-$tag-$platform.tar.gz \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.tar.gz
  

# sha256 checksum
curl \
  --location \
  --output ./avblocks-$tag-$platform.tar.gz.sha256 \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.tar.gz.sha256

# verify sha256 checksum
shasum --check ./avblocks-$tag-$platform.tar.gz.sha256

# unzip
tar -xvf avblocks-$tag-$platform.tar.gz

cd ..
```

## Assets

These demo audio and video assets are used as input for the AVBlocks samples.

```bash
mkdir -p ./assets
cd ./assets

curl \
  --location \
  --output ./avblocks_assets_v5.zip \
  https://github.com/avblocks/avblocks-assets/releases/download/v5/avblocks_assets_v5.zip
  
# unzip
unzip avblocks_assets_v5.zip

cd ..
```
