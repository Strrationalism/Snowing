module JobProcs.PackSound

open Job
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

let Proc = {
    Proc = (fun job ->
        PackSound
            job.ScriptDir
            job.Input.[0]
            (List.tryItem 1 job.Input
            |> function
            | Some x -> x
            | Option.None -> "")
            (GetOptions job.Arguments)
            job.OutputPath)
    InputType = Files
    Command = "PackSound" 
    FinishLogEnabled = true
    Prority = 100
}