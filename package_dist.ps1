param(
    [string]$BuildDir = "build_dist",
    [string]$OutDir = "dist_out",
    [bool]$IncludeDebug = $true,
    [bool]$BuildSamples = $true,
    [bool]$CleanOutDir = $true
)

$ErrorActionPreference = "Stop"

Write-Host "[1/5] Configure"
cmake -S . -B $BuildDir -DGVSDK_INCLUDE_SAMPLE_EXES_IN_INSTALL=$BuildSamples

if ($CleanOutDir -and (Test-Path $OutDir)) {
    Write-Host "[2/5] Clean output directory: $OutDir"
    Remove-Item -Recurse -Force $OutDir
} else {
    Write-Host "[2/5] Skip clean output directory"
}

Write-Host "[3/5] Build + Install Release"
cmake --build $BuildDir --config Release
cmake --install $BuildDir --config Release --prefix $OutDir

if ($IncludeDebug) {
    Write-Host "[4/5] Build + Install Debug"
    cmake --build $BuildDir --config Debug
    cmake --install $BuildDir --config Debug --prefix $OutDir
} else {
    Write-Host "[4/5] Skip Debug"
}

Write-Host "[5/5] Done"
Write-Host "Output: $OutDir"
Write-Host "Release: $OutDir/bin/Release, $OutDir/lib/Release"
if ($IncludeDebug) {
    Write-Host "Debug:   $OutDir/bin/Debug, $OutDir/lib/Debug"
}
