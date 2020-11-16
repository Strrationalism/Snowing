module Bake.Snowing.ConvertSound

open Bake
open Bake.Utils

open System.IO
open WAVFileReader


[<Struct>]
type private Options = {
    Bpm : float32
    BeatsPerBar : uint32
    BeatsOffset : int32
}

let private PackSound scriptPath srcHead srcLoop (options:Options) (dst:string) =
    if File.Exists dst then
        failwith ("Sound file " + dst + " is already exists.")

    let getPath (p:string) =
        if p.Trim() <> "" then
            scriptPath + "\\" + p
        else
            ""
    
    let headFile,loopFile = getPath srcHead , getPath srcLoop

    let GetPCM path =
        let fmt,data = WAVFileReader.ReadFile (File.OpenRead (path))
        if 
            fmt.Channels <> 2us ||
            fmt.DataFormat <> AudioDataFormat.PCM ||
            fmt.SampleRate <> 44100u then
            failwith (path + "is not supposed,it must 44100Hz,2 Channels PCM.")
        let f = File.OpenRead path
        f.Position <- data.Begin
        let buf = Array.init (data.Size |> int) (fun x -> 0uy)
        if f.Read (buf,0,data.Size |> int) <> (data.Size |> int) then
            failwith "Unknown file tail!"
        buf

    let GetPCM2 f =
        if System.String.IsNullOrWhiteSpace f |> not then
            GetPCM f
        else
            [||]
    let headPCM = GetPCM2 headFile
    let loopPCM = GetPCM2 loopFile

    use outf = File.OpenWrite dst
    use out = new BinaryWriter(outf)
    out.Write (headPCM.Length |> uint32)
    out.Write (loopPCM.Length |> uint32)
    out.Write (options.Bpm)
    out.Write (options.BeatsPerBar)
    out.Write (options.BeatsOffset)
    out.Write headPCM
    out.Write loopPCM
    out.Close()
    outf.Close()
    ()



let private GetOptions argumentStrList =
    let defaultOption = {
        Bpm = 120.0f
        BeatsPerBar = 4u
        BeatsOffset = 0
    }

    let optionFolder option (str:string) = 
        match str.Trim() with
        | str when str.EndsWith "BPM" -> {option with Bpm = str.[..str.Length-4].Trim() |> float32 }
        | str when str.EndsWith "BeatPerBar" -> {option with BeatsPerBar = str.[..str.Length - 11].Trim() |> uint32 }
        | str when str.StartsWith "BeatOffset:" -> {option with BeatsOffset = str.[11..].Trim() |> int32 }
        | "" -> option
        | _ -> failwith "Unsuppoted argument."
        

    argumentStrList
    |> List.fold optionFolder defaultOption

[<BakeAction>]
let ConvertSound = {
    help = "转换音频到Snowing引擎支持的格式"
    example = []
    usage = []
    action = fun ctx script ->
        let outDir = script.arguments.Head |> Utils.applyContextToArgument ctx |> normalizeDirPath
        let inDir = script.scriptFile.Directory.FullName |> normalizeDirPath
        let inputFiles = List.last script.arguments |> Utils.applyContextToArgument ctx
        let options = script.arguments.[1..script.arguments.Length - 2] |> List.map (Utils.applyContextToArgument ctx) |> GetOptions
        let tasks = 
            blockArgumentTaskPerLine 
                (fun _ script file ->
                    Utils.mapPathToOutputPath inDir file
                    |> Seq.map (fun (path, fileName) ->
                        let inFile = FileInfo path
                        let outFile = outDir + fileName |> modifyExtensionName "snd"
                        {
                            inputFiles = seq { inFile }
                            outputFiles = seq { outFile }
                            source = script
                            dirty = false
                            run = fun () ->
                                lock stdout (fun () -> printfn "ConvertSound:%s..." inFile.Name)
                                PackSound inFile.Directory.FullName inFile.Name "" options outFile
                        }))
                ctx
                script
                inputFiles

        tasks, ctx
}
