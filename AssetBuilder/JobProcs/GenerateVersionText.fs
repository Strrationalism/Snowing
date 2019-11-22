module JobProcs.GenerateVersionText
open Job

let Proc = {
    Proc = (fun job -> 
        let date = System.DateTime.Now
        sprintf "%02d-%02d | %02d:%02d:%02d" date.Month date.Day date.Hour date.Minute date.Month
        |> fun x -> System.IO.File.WriteAllText(job.OutputPath,x))
    InputType = None
    Command = "GenerateVersionText"
    FinishLogEnabled = false
    Prority = 0
}