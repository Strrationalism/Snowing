module JobProcs.JobProcs
open Job

open System.Collections.Generic


let mutable AllProcs = [
        MakeDir.Proc
        PackSound.Proc
        ConvertTexture.Proc
        PackSprite.Proc
        ConvertFont.Proc
        CopyFile.Proc
        GenerateVersionText.Proc
        RunCommand.FinalRunProc
        RunCommand.PostRunProc
        RunCommand.PreRunProc
    ]

let FindProcByCommand command =
    try
        AllProcs
        |> List.find (fun x -> x.Command = command)
        |> Some
    with
    | :? KeyNotFoundException ->
        Option.None


let private allProcsLock = obj()
let AddCustumProc (cusJob:Job) = 
    lock allProcsLock (fun _ ->
        let CommandProc (job:Job) =
            let args = 
                let a1 =
                    "\"" + job.OutputPath + "\"" ::
                    List.map (fun x -> "\"" + job.ScriptDir.FullName + "\\" + x + "\"") job.Input
                List.concat (seq {yield a1;yield job.Arguments})
            
            let arg = List.reduce (fun x y -> x + " " + y) args
            Utils.StartWait (job.ScriptDir.FullName + "\\" + cusJob.Input.Head) arg
            Utils.WaitForFile 10 job.OutputPath

        let newProc = {
            Proc = CommandProc
            InputType = 
                match cusJob.Arguments.[0] with
                | "File" -> File
                | "Files" -> Files
                | "Dir" -> Directory
                | "None" -> Job.None
                | _ -> failwith "Bad custum tool define."
            Command = cusJob.Arguments.[1]
            FinishLogEnabled = true 
            Prority = 100 }
        AllProcs <- newProc :: AllProcs)

let CustumJobProc = {
    Proc = AddCustumProc
    InputType = File
    Command = "Custum"
    FinishLogEnabled = false
    Prority = 0 }

AllProcs <- CustumJobProc :: AllProcs

