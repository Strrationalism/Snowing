module Utils

open System.Diagnostics
open System.IO
open System.Threading

let StartWait exe arg =
    use prc = new Process()
    prc.StartInfo.FileName <- exe
    prc.StartInfo.WorkingDirectory <- (FileInfo exe).DirectoryName
    prc.StartInfo.Arguments <- arg
    prc.StartInfo.WindowStyle <- ProcessWindowStyle.Hidden
    if prc.Start() |> not then
        failwith ("Can not start " + exe)
    prc.WaitForExit ()

let WaitForFile maxTimes path =
    for _ in 0..maxTimes do
        if File.Exists path |> not then
            Thread.Sleep 1000
    if File.Exists path |> not then
        failwith ("Can not find file " + path)
