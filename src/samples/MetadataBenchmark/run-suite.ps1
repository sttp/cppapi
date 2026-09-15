param(
    [string]$Executable = '',
    [string]$Label = 'baseline',
    [int]$TimeoutSeconds = 180
)
$ErrorActionPreference = 'Stop'
$repo = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
if (!$Executable) { $Executable = Join-Path $repo 'build/startup-diagnostics/MetadataBenchmark.exe' }
$Executable = (Resolve-Path -LiteralPath $Executable).Path
if ($Label -notmatch '^[A-Za-z0-9_-]+$') { throw 'Label must contain only letters, numbers, underscore or hyphen' }
$output = Join-Path $repo "build/startup-diagnostics/$Label"
New-Item -ItemType Directory -Force -Path $output | Out-Null
$cases = @(
    @('analog','1','5000','1'),
    @('analog','1','10000','1'),
    @('analog','1','20000','1'),
    @('analog','1','50000','1'),
    @('analog','500','100','1'),
    @('digital','1','20000','1'),
    @('phasor','1','5000','1'),
    @('analog','1','10','6000'),
    @('analog','500','100','1','gzip')
)
$failures = 0
$oldTrace = $env:STTP_STARTUP_TRACE
$env:STTP_STARTUP_TRACE = '1'
try {
    foreach ($arguments in $cases) {
        $name = $arguments -join '-'
        $stdout = Join-Path $output "$name.out"
        $stderr = Join-Path $output "$name.log"
        $process = Start-Process -FilePath $Executable -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
        $elapsed = [System.Diagnostics.Stopwatch]::StartNew()
        while (!$process.WaitForExit(1000)) {
            if ($elapsed.Elapsed.TotalSeconds -ge $TimeoutSeconds) {
                $process.Kill()
                $process.WaitForExit()
                break
            }
        }
        $process.Refresh()
        if ($process.ExitCode -ne 0) {
            $failures++
            Write-Output "$name FAILED or timed out after $($elapsed.Elapsed.TotalSeconds.ToString('F1')) seconds, exit $($process.ExitCode)"
            Get-Content -LiteralPath $stderr -Tail 3
        } else {
            Get-Content -LiteralPath $stdout | Where-Object { $_ -like 'RESULT,*' }
        }
        $process.Dispose()
    }
} finally { $env:STTP_STARTUP_TRACE = $oldTrace }

if ($failures -gt 0) { throw "$failures benchmark cases failed; see $output" }

