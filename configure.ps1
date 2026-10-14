# get script dir
$script_dir=$PSScriptRoot

# PROCESSOR_ARCHITEW6432 is set when PowerShell runs emulated (e.g. x64 PowerShell on ARM64).
$host_arch = if ($env:PROCESSOR_ARCHITEW6432) { $env:PROCESSOR_ARCHITEW6432 } else { $env:PROCESSOR_ARCHITECTURE }
$vs_host_arch = if ($host_arch -eq "ARM64") { "arm64" } else { "amd64" }

Write-Output "Host architecture: $host_arch"
Write-Output "Visual C++ environment: -arch=$vs_host_arch -host_arch=$vs_host_arch"

# setup Visual C++ environment for the host
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2022

$vs_install_dir = $(Get-VSSetupInstance | Select-VSSetupInstance -Version '[17.0,18.0]' | Select-Object -ExpandProperty InstallationPath)
$vs_common_tools = "${vs_install_dir}/Common7/Tools/"

& $script_dir/scripts/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-arch=$vs_host_arch -host_arch=$vs_host_arch"

Write-Output "Visual Studio: $vs_install_dir"
Write-Output "Compiler: $((Get-Command cl.exe -ErrorAction SilentlyContinue).Source)"

# create build dirs
New-Item -Force -ItemType Directory $script_dir/build/debug_x64
New-Item -Force -ItemType Directory $script_dir/build/debug_arm64
New-Item -Force -ItemType Directory $script_dir/build/release_x64
New-Item -Force -ItemType Directory $script_dir/build/release_arm64

New-Item -Force -ItemType Directory $script_dir/build/debug_demo_x64
New-Item -Force -ItemType Directory $script_dir/build/debug_demo_arm64
New-Item -Force -ItemType Directory $script_dir/build/release_demo_x64
New-Item -Force -ItemType Directory $script_dir/build/release_demo_arm64
