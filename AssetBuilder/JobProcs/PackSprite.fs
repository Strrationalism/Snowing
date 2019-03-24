module JobProcs.PackSprite

open System.IO
open Utils
open Job
open Newtonsoft.Json
open Newtonsoft.Json.Linq
open ConvertTexture

let CallTextureMerger inputDir outputFilePair =
    let textureMerger =
        [
            for i in 'C'..'Z' ->
                string i + ":\\Program Files\\Egret\\TextureMerger\\TextureMerger.exe" ]
        |> List.filter File.Exists
        |> function
        | s :: _ -> s
        | [] ->
            failwith "Can not find TextureMerger"
    sprintf "-p \"%s\" -o \"%s.json\"" inputDir outputFilePair
    |> StartWait textureMerger
    WaitForFile 5 (outputFilePair + ".json")
    WaitForFile 5 (outputFilePair + ".png")

let private PackSprite (job:Job) =
    CallTextureMerger (job.ScriptDir.FullName + "\\" + job.Input.Head) job.OutputPath
    let frames =
        using (new JsonTextReader(File.OpenText(job.OutputPath + ".json"))) (fun x ->
            let token = JToken.ReadFrom x
            let frames = token.["frames"]
            [ for i in frames -> 
                uint16 i.Path.[7..],
                {
                    x = uint16 i.First.["x"]
                    y = uint16 i.First.["y"]
                    w = uint16 i.First.["w"]
                    h = uint16 i.First.["h"]
                }])
        |> List.sortBy (fun (a,_) -> a)

    let spriteFrames =
        let maxFrameID =
            frames
            |> List.maxBy (fun (a,_) -> a)
            |> fst
        let ret =
            Array.init (maxFrameID |> int |> (+) 1) (fun x -> { x = 0us; y = 0us; w = 0us;h = 0us })
        frames
        |> List.iter (fun (id,frame) ->
            ret.[int id] <- frame)
        ret |> Array.toList

    let newJob = {
        Processor = JobProcs.ConvertTexture.Proc
        Input = [ job.OutputPath + ".png" ]
        OutputPath = job.OutputPath
        Arguments = job.Arguments
        ScriptDir = job.ScriptDir
    }
    
    JobProcs.ConvertTexture.ConvertTexture spriteFrames true newJob

    File.Delete (job.OutputPath + ".json")
    File.Delete (job.OutputPath + ".png")
    ()

let Proc = {
    Proc = PackSprite
    InputType = Directory
    Command = "PackSprite"
    FinishLogEnabled = true
    Prority = 100
}
