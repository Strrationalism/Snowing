module JobProcs.MakeDir

open Job

let Proc : JobProc = {
    Proc = (fun job ->
        job.OutputPath
        |> System.IO.Directory.CreateDirectory
        |> ignore)
        
    InputType = None
    Command = "MakeDir"  
    FinishLogEnabled = false
    Prority = 0
}

