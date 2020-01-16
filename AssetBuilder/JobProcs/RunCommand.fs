module JobProcs.RunCommand

open Job

let private runCommandProc job =
    Utils.StartWait (job.ScriptDir.FullName + "\\" + job.Arguments.[0]) 
        (job.Input
        |> List.reduce (fun a b -> a + " " + b))

let FinalRunProc = {
    Proc = runCommandProc
    InputType = InputType.None
    Command = "FinalRun"
    FinishLogEnabled = false
    Prority = System.Int32.MaxValue
}

let PostRunProc = {
    FinalRunProc with
        Command = "PostRun"
        Prority = System.Int32.MaxValue - 1
}

let PreRunProc = {
    FinalRunProc with
        Command = "PreRun"
        Prority = 0
}

