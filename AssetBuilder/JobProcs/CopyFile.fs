module JobProcs.CopyFile

open System.IO
open Job

let Proc = {
    Proc = (fun job -> 
        File.Copy (job.ScriptDir + "\\" + job.Input.Head,job.OutputPath))
    InputType = InputType.File
    Command = "CopyFile"
    FinishLogEnabled = true
    Prority = 100
}


