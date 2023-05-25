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
$totalLogicalCores = (
    (Get-CimInstance -ClassName Win32_Processor).NumberOfLogicalProcessors 
    | Measure-Object -Sum
).Sum

# Build torch test
New-Item ./build -ItemType Directory -ErrorAction Ignore
Set-Location build -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake .. } -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake --build . --parallel $(($totalLogicalCores / 2)) } -ErrorAction Stop
Invoke-Call -ScriptBlock { cmake -P .\CmakeCopyDLLsDebug.cmake } -ErrorAction Stop

# Run torch test
Invoke-Call -ScriptBlock { ./Debug/TorchTest.exe } -ErrorAction Stop