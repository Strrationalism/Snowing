﻿open System.IO
open System
open System.Diagnostics


[<EntryPoint>]
let main args =
    try
        let timer = Stopwatch ()
        timer.Start ()
        try
            Console.SetWindowSize(90,40)
        with _ -> ()
        let OutputPath =
            if Directory.Exists (Array.head args) then
                DirectoryInfo(Array.head args)
            else
                Directory.CreateDirectory (Array.head args)

        let Jobs = 
            BuildScriptParser.ParseBuildScript OutputPath.FullName "BuildScript.txt" 
            
        ProcRunner.RunProcs Jobs (OutputPath.FullName)

        printfn ""
        printfn "Build Time:%A" timer.Elapsed
        0
    with
    | :?FileNotFoundException ->
        printfn "BuildScript.txt not found!"
        -1
    | e ->
        printfn "Error:"
        printfn "%A" e
        printfn "========="
        printfn "Command Line: AssetBuilder outputPath"
        -1
