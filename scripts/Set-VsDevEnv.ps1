param(
    # target architecture: x64 or arm64
    [ValidateSet("x64", "arm64")]
    [String]$arch = "x64"
)

# Sets up the Visual C++ command line environment (cl, link, LIB, INCLUDE, ...) in the
# current process for the given target architecture. Works on both AMD64 and ARM64 hosts:
# the native host tools are used to compile for (or cross compile to) the target.
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2022

# OSArchitecture reports the real OS architecture even when PowerShell itself runs
# emulated (e.g. x64 PowerShell on an ARM64 machine, where PROCESSOR_ARCHITECTURE says AMD64).
$os_arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture

switch ($os_arch) {
    "X64"   { $vs_host_arch = "amd64" }
    "Arm64" { $vs_host_arch = "arm64" }
    default {
        Write-Error "Unsupported Windows host architecture: $os_arch"
        exit 1
    }
}

$vs_arch = if ($arch -eq "x64") { "amd64" } else { "arm64" }

$vs_install_dir = $(Get-VSSetupInstance | Select-VSSetupInstance -Version '[17.0,18.0]' | Select-Object -ExpandProperty InstallationPath)
$vs_common_tools = "${vs_install_dir}/Common7/Tools/"

# VsDevCmd looks up vswhere.exe on PATH
$vs_installer_dir = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer"
if ((Test-Path -LiteralPath $vs_installer_dir) -and -not ($env:PATH -split ";" -contains $vs_installer_dir)) {
    $env:PATH = "$vs_installer_dir;$env:PATH"
}

& $PSScriptRoot/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-no_logo -arch=$vs_arch -host_arch=$vs_host_arch"
