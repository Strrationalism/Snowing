module Bake.Snowing.ConvertTexture

open System.IO
open SixLabors
open SixLabors.ImageSharp
open System.Diagnostics
open System.Threading
open System

open Bake

type TextureFormat =
| R8G8B8A8_UNORM = 0uy
| R8_UNORM = 1uy
| DDS = 2uy

let StartWait exe arg =
    use prc = new Process()
    prc.StartInfo.FileName <- exe
    prc.StartInfo.WorkingDirectory <- (FileInfo exe).DirectoryName
    prc.StartInfo.Arguments <- arg
    prc.StartInfo.WindowStyle <- ProcessWindowStyle.Hidden
    prc.StartInfo.RedirectStandardOutput <- true
    if prc.Start() |> not then
        failwith ("Can not start " + exe)
    prc.WaitForExit ()
    match prc.ExitCode with
    | 0 -> ()
    | x ->
        failwith (
            "Build Tool Failed. Exit Code:" + string x + 
            Environment.NewLine + 
            exe + " " + arg + 
            Environment.NewLine +
            prc.StandardOutput.ReadToEnd())

let WaitForFile maxTimes path =
    for _ in 0..maxTimes do
        if File.Exists path |> not then
            Thread.Sleep 1000
    if File.Exists path |> not then
        failwith ("Can not find file " + path)


type RequestedTextureFormat =
| RGBA32
| R8
| DDS of string

type Job = {
    ScriptDir : DirectoryInfo
    Input : string list
    Arguments : string list
    OutputPath : string
}

let Is4MulBitmap (bitmap: Image) =
    bitmap.Width % 4 = 0 && bitmap.Height % 4 = 0

let WrapBitmapTo4 (bitmap: Image<ImageSharp.PixelFormats.Rgba32>) =
    let wrapTo4 x =
        (4 - (x % 4)) % 4 + x

    let newSize =
        (wrapTo4 bitmap.Width,wrapTo4 bitmap.Height)
    let newBitmap = new Image<ImageSharp.PixelFormats.Rgba32>(fst newSize,snd newSize)
    for y in 0..bitmap.Height-1 do
        for x in 0..bitmap.Width-1 do
            let px = bitmap.[x, y]
            newBitmap.[x,y] <- px
    newBitmap


let ParseArgument (job : Job) =
    match job.Arguments.Head with
    | "RGBA32" -> RGBA32
    | "R8" -> R8
    | "DDS" -> DDS(job.Arguments.[1])
    | x -> failwith ("Unknown requested texture format:" + x)

let GetFormatFormRequestedFormat = function
| RGBA32 -> TextureFormat.R8G8B8A8_UNORM
| R8 -> TextureFormat.R8_UNORM
| DDS _ -> TextureFormat.DDS

type SpriteFrame = {
    x : uint16
    y : uint16
    w : uint16
    h : uint16 
}

type TextureHead = {
    Format : TextureFormat 
    Width : uint16
    Height : uint16
    SpriteSheet : SpriteFrame seq
}

let SaveTextureHead head (stream:BinaryWriter) =
    stream.Write (uint8 head.Format)
    stream.Write (uint16 head.Width)
    stream.Write (uint16 head.Height)
    stream.Write (head.SpriteSheet |> Seq.length |> uint16)
    head.SpriteSheet
    |> Seq.iter (fun x -> 
        stream.Write (float32 x.x / float32 head.Width)
        stream.Write (float32 x.y / float32 head.Height)
        stream.Write (float32 x.w / float32 head.Width)
        stream.Write (float32 x.h / float32 head.Height))

let DumpPixels (format:TextureFormat) (path:string) : uint16*uint16*byte[] =
    let pixelSize =
        match format with
        | TextureFormat.R8G8B8A8_UNORM -> 4uy
        | TextureFormat.R8_UNORM -> 1uy
        | _ -> raise (System.ArgumentException())

    use bitmap = Image.Load<PixelFormats.Rgba32>(path)
    let pixels = Array.init ((pixelSize |> int) * bitmap.Width * bitmap.Height) (fun _ -> 0uy)
    use memStream = new MemoryStream(pixels)
    use binWriter = new BinaryWriter(memStream)
    for y in 0..bitmap.Height-1 do
        for x in 0..bitmap.Width-1 do
            let px = bitmap.[x, y]
            match format with
            | TextureFormat.R8G8B8A8_UNORM ->
                binWriter.Write px.R
                binWriter.Write px.G
                binWriter.Write px.B
                binWriter.Write px.A
            | TextureFormat.R8_UNORM ->
                binWriter.Write px.R
            | _ -> raise (System.ArgumentException())
    binWriter.Flush ()
    binWriter.Close ()
    memStream.Close ()
    bitmap.Width |> uint16,bitmap.Height |> uint16,pixels

let ConvertTextureFunc spriteSheet inputFullName (job:Job) =
    let reqFormat = ParseArgument job
    let innerFormat = GetFormatFormRequestedFormat reqFormat

    use dstFile = File.OpenWrite job.OutputPath
    use dstWriter = new BinaryWriter(dstFile)

    let srcPath =
        match inputFullName with
        | false -> job.ScriptDir.FullName + "\\" + job.Input.Head
        | true -> job.Input.Head

    match reqFormat with
    | RGBA32 | R8 -> 
        let w,h,px = DumpPixels innerFormat srcPath
        let head = {
            Format = innerFormat
            Width = uint16 w
            Height = uint16 h
            SpriteSheet = spriteSheet
        }
        
        SaveTextureHead head dstWriter
        dstWriter.Write px

    | DDS (ddsFormat) -> 
        let orgBitmap = Image.Load<PixelFormats.Rgba32>(srcPath)
        let is4MulBitmap = Is4MulBitmap orgBitmap
        use bitmap = 
            if is4MulBitmap then orgBitmap
            else 
                let ret = orgBitmap |> WrapBitmapTo4
                orgBitmap.Dispose()
                ret

        let tmpout = FileInfo(job.OutputPath).DirectoryName
        let texconv = 
            System.Reflection.Assembly.GetExecutingAssembly().Location 
            |> FileInfo |> fun x -> x.DirectoryName |> Utils.normalizeDirPath
        let texconv = texconv + "texconv.exe"

        let incPng =
            if is4MulBitmap then
                FileInfo(srcPath).FullName
            else
                let path = job.OutputPath + ".png"
                bitmap.Save path
                path

        sprintf "\"%s\" -ft DDS -f %s -o \"%s\"" incPng ddsFormat tmpout
        |> StartWait texconv

        if not is4MulBitmap then
            File.Delete incPng
       
        let head = {
            Format = TextureFormat.DDS
            Width = uint16 bitmap.Width
            Height = uint16 bitmap.Height
            SpriteSheet = spriteSheet
        }
        SaveTextureHead head dstWriter
        let ddsPath = 
            let baseFileName = 
                FileInfo(srcPath).Name
            tmpout + "\\" + baseFileName.[..baseFileName.LastIndexOf '.' - 1] + ".DDS"
        WaitForFile 5 ddsPath
        let ddsBytes = File.ReadAllBytes ddsPath
        dstWriter.Write (ddsBytes |> Array.length |> uint32)
        dstWriter.Write ddsBytes
        File.Delete ddsPath

    dstWriter.Close ()
    dstFile.Close ()

[<BakeAction>]
let ConvertTexture = {
    help = "转换纹理到Snowing引擎可以使用的格式"
    usage = []
    example = []
    action = fun ctx script ->
        let outDir = script.arguments.Head |> Utils.applyContextToArgument ctx |> Utils.normalizeDirPath
        let arguments = script.arguments.[1..script.arguments.Length-2] |> List.map (Utils.applyContextToArgument ctx)
        let inputFiles = List.last script.arguments |> Utils.applyContextToArgument ctx

        let tasks =
            Utils.blockArgumentTaskPerLine (fun _ script inputFiles ->
                Utils.mapPathToOutputPath script.scriptFile.Directory.FullName inputFiles
                |> Seq.map (fun (path, fileName) ->
                    let outFile = outDir + fileName |> Utils.modifyExtensionName "ctx"
                    {
                        inputFiles = seq { FileInfo path }
                        source = script
                        outputFiles = seq { outFile }
                        dirty = false
                        run = fun () ->
                            lock stdout (fun () -> printfn "ConvertTexture:%s..." fileName)
                            ConvertTextureFunc Seq.empty true {
                                ScriptDir = script.scriptFile.Directory
                                Input = [ path ]
                                OutputPath = outFile
                                Arguments = arguments
                            }

                    }))
                ctx
                script
                inputFiles

        tasks, ctx
}