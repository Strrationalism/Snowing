module BuildScriptParser

open JobProcs

let rec ParseBuildScript baseOutputPath scriptFilePath =

    let scriptInfo =
        System.IO.FileInfo scriptFilePath

    let scriptDir = 
        match scriptFilePath.LastIndexOf '\\' with
        | -1 -> "."
        | x -> scriptFilePath.[..x-1]

    let makeJob command args input output : Job.Job = {
        Processor = JobProcs.FindProcByCommand command |> Option.get
        Input = input |> Array.toList
        OutputPath = baseOutputPath + "\\" + output
        Arguments = args |> Array.toList
        ScriptDir = scriptDir}

    scriptFilePath
    |> System.IO.File.ReadAllLines
    |> Array.map (fun x -> x + "#")
    |> Array.map (fun x -> x.[0..x.IndexOf '#' - 1])
    |> Array.map (fun x -> x.Trim())
    |> Array.filter (not << System.String.IsNullOrWhiteSpace)
    |> Array.map (fun x -> x.Replace(" ",""))
    |> Array.map (fun x -> x.Replace("\t",""))
    |> Array.map (fun x ->
        let pos =
            match x.IndexOf '(',x.IndexOf '[' with
            | a,-1 -> a
            | -1,b -> b
            | a,_ -> a
        x.[0..pos-1],x.[pos..])

    |> Array.map (fun (command,args) ->
        match args.IndexOf '(' with
        | -1 -> command,"",args
        | pos -> command,args.[pos+1..args.IndexOf ')' - 1],args.[args.IndexOf ')'+1..])

    |> Array.map (fun (command,args,io) ->
        let iPos = io.IndexOf '['
        let oPos = io.IndexOf ']'
        command,args,io.[iPos+1..oPos-1],io.[oPos+1..])

    |> Array.map (fun (command,args,input,output) ->
        let p = output.IndexOf '['
        let output =
            match p with
            | -1 -> ""
            | x ->
                let q = output.IndexOf ']'
                output.[p+1..q-1]
        command,args,input,output)

    |> Array.map (fun (command,args,input,output) ->
        command.Trim(),
        args.Split ',' |> Array.map (fun x -> x.Trim()),
        input.Split ',' |> Array.map (fun x -> x.Trim()),
        output.Trim())

    |> Array.collect (fun (cmd,args,i,o) ->
        match cmd with
        | "Include" -> 
            scriptInfo.DirectoryName + "\\" + (Array.head i)
            |> ParseBuildScript baseOutputPath
        | "Batch" ->
            System.IO.Directory.EnumerateFiles(scriptInfo.DirectoryName,i.[0],System.IO.SearchOption.AllDirectories)
            |> Seq.cast
            |> Seq.map (fun (x:string) -> x.[1+scriptInfo.DirectoryName.Length..])
            |> Seq.map (fun path -> 
                let fileName = 
                    let n =
                        System.IO.FileInfo(path).Name
                    o + "\\" + n.[.. -1 + n.IndexOf '.'] + "." + args.[1]
                makeJob args.[0] args.[2..] [|path|] fileName)
            |> Seq.toArray

        | _ when JobProcs.JobProcs.FindProcByCommand cmd |> Option.isSome ->
            let j = makeJob cmd args i o
            match j.Processor.Command with
            | "Custum" ->
                j.Processor.Proc j
                [||]
            | _ -> [| j |]
        | x -> failwith ("Unknown command:" + x))

    