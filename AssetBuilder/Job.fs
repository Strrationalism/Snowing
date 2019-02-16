module Job

open System.IO

type InputType =
| File
| Files
| Directory
| None

type JobProc = {
    Proc : Job -> unit
    InputType : InputType
    Command : string
    FinishLogEnabled : bool
}

and Job = {
    Processor : JobProc
    Input : string list
    OutputPath : string
    Arguments : string list 
    ScriptDir : DirectoryInfo
}


