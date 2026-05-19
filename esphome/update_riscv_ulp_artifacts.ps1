param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('esp32s2', 'esp32s3')]
    [string] $Target,

    [Parameter(Mandatory = $false)]
    [string] $BuildDir
)

$ErrorActionPreference = 'Stop'

$targetSuffixMap = @{
    'esp32s2' = 'Esp32S2'
    'esp32s3' = 'Esp32S3'
}

$targetSuffix = $targetSuffixMap[$Target]

if (-not $BuildDir) {
    $BuildDir = Join-Path $PSScriptRoot '..\riscv_blink\build\esp-idf\riscv_blink\ulp_main'
}

$BuildDir = [System.IO.Path]::GetFullPath($BuildDir)
$binPath = Join-Path $BuildDir 'ulp_main.bin'
$ldPath = Join-Path $BuildDir 'ulp_main.ld'
$headerPath = Join-Path $PSScriptRoot "my_components\riscv_blink\ulp_artifacts_${Target}.h"

if (-not (Test-Path $binPath)) {
    throw "Missing ULP binary: $binPath"
}

if (-not (Test-Path $ldPath)) {
    throw "Missing ULP linker symbols: $ldPath"
}

$bytes = [System.IO.File]::ReadAllBytes($binPath)
if ($bytes.Length -eq 0) {
    throw "ULP binary is empty: $binPath"
}

$ldLines = Get-Content $ldPath
$symbols = @{}
foreach ($line in $ldLines) {
    if ($line -match '^(?<name>[A-Za-z0-9_]+)\s*=\s*(?<addr>0x[0-9a-fA-F]+);$') {
        $symbols[$matches.name] = $matches.addr.ToLowerInvariant()
    }
}

$required = @(
    'ulp_run_count',
    'ulp_flash_lp_io_inverted',
    'ulp_pulse_width_us',
    'ulp_flash_lp_io'
)

foreach ($name in $required) {
    if (-not $symbols.ContainsKey($name)) {
        throw "Missing required symbol '$name' in $ldPath"
    }
}

$formattedBytes = @()
for ($index = 0; $index -lt $bytes.Length; $index += 12) {
    $chunk = $bytes[$index..([Math]::Min($index + 11, $bytes.Length - 1))]
    $formattedBytes += '    ' + (($chunk | ForEach-Object { ('0x{0:x2}' -f $_) }) -join ', ')
}

$arrayBody = ($formattedBytes -join ",`r`n")

$content = @"
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace esphome {
namespace riscv_blink {

inline constexpr std::array<uint8_t, $($bytes.Length)> kUlpMainBinary${targetSuffix} = {
$arrayBody
};
inline constexpr bool kUlpMainArtifact${targetSuffix}Valid = true;
inline constexpr uintptr_t kUlpMainRunCountAddr${targetSuffix} = $($symbols['ulp_run_count'])u;
inline constexpr uintptr_t kUlpMainFlashLpIoInvertedAddr${targetSuffix} = $($symbols['ulp_flash_lp_io_inverted'])u;
inline constexpr uintptr_t kUlpMainPulseWidthUsAddr${targetSuffix} = $($symbols['ulp_pulse_width_us'])u;
inline constexpr uintptr_t kUlpMainFlashLpIoAddr${targetSuffix} = $($symbols['ulp_flash_lp_io'])u;

}  // namespace riscv_blink
}  // namespace esphome
"@

Set-Content -Path $headerPath -Value $content -NoNewline
Write-Output "Updated $headerPath from $BuildDir"