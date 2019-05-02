module JobProcs.PackSound

open Job
open System.IO
open WAVFileReader


[<Struct>]
type private Options = {
    Bpm : float32 voption
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
    out.Write (options.Bpm.IsSome)
    out.Write (if options.Bpm.IsSome then options.Bpm.Value else 120.0f)
    out.Write headPCM
    out.Write loopPCM
    out.Close()
    outf.Close()
    ()



let private GetOptions argumentStrList =
    let defaultOption = {
        Bpm = ValueNone
    }

    let optionFolder option (str:string) = 
        match str with
        | str when str.EndsWith "BPM" -> {option with Bpm = str.[..str.Length-4] |> float32 |> ValueSome }
        | "" -> option
        | _ -> failwith "Unsuppoted argument."
        

    argumentStrList
    |> List.fold optionFolder defaultOption

let Proc = {
    Proc = (fun job ->
        PackSound
            job.ScriptDir.FullName
            job.Input.[0]
            job.Input.[1]
            (GetOptions job.Arguments)
            job.OutputPath)
    InputType = Files
    Command = "PackSound" 
    FinishLogEnabled = true
    Prority = 100
}