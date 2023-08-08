# Auxiliary function to stop on failure
function Invoke-Call {
    param (
        [scriptblock]$ScriptBlock,
        [string]$ErrorAction = $ErrorActionPreference
    )
    & @ScriptBlock
    if (($lastexitcode -ne 0) -and $ErrorAction -eq "Stop") {
        Pop-Location
        exit $lastexitcode
    }
}

# Compute total number of cores to set for cmake --build
$totalLogicalCores = ((Get-CimInstance -ClassName Win32_Processor).NumberOfLogicalProcessors | Measure-Object -Sum).Sum

# Pull changes from remote repository
Invoke-Call -ScriptBlock { git pull } -ErrorAction Stop

# Save initial location
Push-Location

# Build SmartCar
New-Item ./build -ItemType Directory -ErrorAction Ignore
Set-Location build -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake .. } -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake --build . --parallel $(($totalLogicalCores / 2)) } -ErrorAction Stop

# Run SmartCar executable
Invoke-Call -ScriptBlock { ./SmartCarMain.exe ../config.json } -ErrorAction Stop

# Return to initial location
Pop-Location -ErrorAction Stop