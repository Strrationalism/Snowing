﻿module Bake.Snowing.BuildFont

open Bake
open Bake.Snowing.ConvertTexture

open System.IO
open System.Text

type CharInfo = {
    Ch : char
    FaceID : uint16
    Sprite : SpriteFrame
}

let private ParseFontIndex txtPath =
    txtPath
    |> File.ReadAllLines
    |> Array.map (fun x -> x.Trim())
    |> Array.filter (System.String.IsNullOrWhiteSpace >> not)
    |> Array.map (fun x -> x.[0],x.[1..].Trim().Split '\t')
    |> Array.map (fun (ch,strs) ->
        let info =
            strs
            |> Array.map (fun x ->
                x.Split '='
                |> Array.item 1
                |> uint16)
        ch,info)
    |> Array.map (fun (ch,info) ->{
        Ch = ch
        FaceID = info.[0]
        Sprite = {
            x = info.[1]
            y = info.[2]
            w = info.[3]
            h = info.[4]
        }})
    |> Array.groupBy (fun x -> x.FaceID)
    |> dict

let private ConvertFont (job:Job) =
    let fontIndex = ParseFontIndex (job.ScriptDir.FullName + "\\" + job.Input.Head + "\\font.txt")
    let biggestFaceID = fontIndex.Keys |> Seq.max

    [|0us..biggestFaceID|]
    |> Array.Parallel.iteri (fun _ faceID ->
        let chars = fontIndex.[faceID]
        let ctxJob = {
            Input = [job.ScriptDir.FullName + "\\" + job.Input.Head + "\\font" + string faceID + ".bmp"]
            OutputPath = job.OutputPath + "-" + string faceID + ".tmp"
            Arguments = job.Arguments
            ScriptDir = job.ScriptDir
        }

        ConvertTextureFunc (chars |> Array.map (fun x -> x.Sprite)) true ctxJob)

    let charCount = fontIndex.Values |> Seq.map Array.length |> Seq.sum 
    let faceCount = fontIndex.Keys.Count

    let faceInfoChunks = Array.init (12*faceCount) (fun _ -> 0uy)

    use out = File.OpenWrite job.OutputPath
    use writer = new BinaryWriter(out)
    writer.Write (uint16 charCount)
    writer.Write (uint16 faceCount)

    writer.Write faceInfoChunks

    fontIndex.Values
    |> Seq.iter (
        Array.iteri (fun i ch ->
            writer.Write (Encoding.Unicode.GetBytes (string ch.Ch))
            writer.Write (uint16 ch.FaceID)
            writer.Write (uint16 i)))

    use faceInfosStream = new MemoryStream(faceInfoChunks)
    use faceInfosWriter = new BinaryWriter(faceInfosStream)
    for faceID in 0us..biggestFaceID do
        let tmpFilePath = job.OutputPath + "-" + string faceID + ".tmp"
        let bytes = File.ReadAllBytes tmpFilePath
        File.Delete tmpFilePath

        faceInfosWriter.Write (uint32 faceID)
        faceInfosWriter.Write (writer.BaseStream.Position |> uint32)
        faceInfosWriter.Write (bytes |> Array.length |> uint32)
        writer.Write bytes
    faceInfosWriter.Flush ()
    faceInfosWriter.Close ()
    faceInfosStream.Close ()

    writer.BaseStream.Position <- 4L
    writer.Write faceInfoChunks
    writer.Flush ()
    writer.Close ()
    out.Close ()

[<BakeAction>]
let BuildFont = {
    help = "从图片字模构建字体文件"
    usage = []
    example = []
    action = fun ctx script ->
        Utils.verifyArgumentCount script 2
        let out = script.arguments.[0] |> Utils.applyContextToArgument ctx
        let input = script.arguments.[1] |> Utils.applyContextToArgument ctx 

        seq { {
            dirty = false
            source = script
            inputFiles = Utils.normalizeDirPath script.scriptFile.Directory.FullName + input |> Directory.EnumerateFiles |> Seq.map FileInfo
            outputFiles = seq { out }
            run = fun () ->
                lock stdout (fun () -> printfn "BuildFont:%s..." input)
                ConvertFont {
                    Arguments = [ "R8" ]
                    Input = [ input ]
                    ScriptDir = script.scriptFile.Directory
                    OutputPath = out
                }
        } },
        ctx

}

