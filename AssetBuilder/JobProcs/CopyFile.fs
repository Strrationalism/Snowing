module JobProcs.CopyFile

open System.IO
open Job

let Proc = {
    Proc = (fun job -> 
        File.ReadAllBytes (job.ScriptDir.FullName + "\\" + job.Input.Head)
        |> fun bytes -> File.WriteAllBytes(job.OutputPath,bytes))
    InputType = InputType.File
    Command = "CopyFile"
    FinishLogEnabled = true
    Prority = 100
}


