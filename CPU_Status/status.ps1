$port= new-Object System.IO.Ports.SerialPort COM46,115200,None,8,one
$port.open()

$totalRam = (Get-CimInstance Win32_PhysicalMemory | Measure-Object -Property capacity -Sum).Sum
while($true) {
    $date = Get-Date -Format "dd-MM-yyyy HH:mm:ss"
    $cpuTime = (Get-Counter '\Processor(_Total)\% Processor Time').CounterSamples.CookedValue
    $availMem = (Get-Counter '\Memory\Available MBytes').CounterSamples.CookedValue
    $port.WriteLine($date + ' CPU:' + $cpuTime.ToString("#,00.0") + '% Free Mem:  ' + $availMem.ToString("F0") + ' MB     ' + (104857600 * $availMem / $totalRam).ToString("F0") + '%  ')
    #Start-Sleep -s 2
}