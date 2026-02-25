param(
    [string]$BuildDir = "build_dist",
    [string]$OutDir = "dist_out",
    [bool]$BuildSamples = $true,
    [bool]$CleanOutDir = $true
)

$ErrorActionPreference = "Stop"
$buildSamplesValue = if ($BuildSamples) { "ON" } else { "OFF" }

Write-Host "[1/4] Configure (Release-only public package)"
cmake -S . -B $BuildDir `
    -DGVSDK_INCLUDE_SAMPLE_EXES_IN_INSTALL=$buildSamplesValue

if ($CleanOutDir -and (Test-Path $OutDir)) {
    Write-Host "[2/4] Clean output directory: $OutDir"
    Remove-Item -Recurse -Force $OutDir
} else {
    Write-Host "[2/4] Skip clean output directory"
}

Write-Host "[3/4] Build + Install Release"
cmake --build $BuildDir --config Release
cmake --install $BuildDir --config Release --prefix $OutDir

Write-Host "[4/4] Done"
Write-Host "Output: $OutDir"
Write-Host "Release: $OutDir/bin/Release, $OutDir/lib/Release"
