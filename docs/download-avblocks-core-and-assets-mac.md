# Download AVBlocks Core and Assets on macOS

Change to the directory where you cloned this repository:

```bash
cd avblocks-cpp
```

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

```bash
# select version and platform
tag="v3.1.0-demo.1"
platform="darwin"

# download
mkdir -p ./sdk
cd ./sdk

# sdk
curl \
  --location \
  --output ./avblocks-$tag-$platform.zip \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip
  
# sha256 checksum
curl \
  --location \
  --output ./avblocks-$tag-$platform.zip.sha256 \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks-$tag-$platform.zip.sha256

# verify sha256 checksum
shasum --check ./avblocks-$tag-$platform.zip.sha256

# unzip
unzip avblocks-$tag-$platform.zip

cd ..
```

## Assets

These demo audio and video assets are used as input for the AVBlocks samples.

```bash
mkdir -p ./assets
cd ./assets

curl \
  --location \
  --output ./avblocks_assets_v2.zip \
  https://github.com/avblocks/avblocks-assets/releases/download/v1/avblocks_assets_v2.zip
  
# unzip
unzip avblocks_assets_v2.zip

cd ..
```
