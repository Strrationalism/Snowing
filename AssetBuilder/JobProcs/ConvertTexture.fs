module JobProcs.ConvertTexture

open Job
open System.IO
open System.Drawing
open System

type TextureFormat =
| R8G8B8A8_UNORM = 0uy
| R8_UNORM = 1uy
| DDS = 2uy

type RequestedTextureFormat =
| RGBA32
| R8
| DDS of string

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

    use bitmap = new Bitmap(path)
    let pixels = Array.init ((pixelSize |> int) * bitmap.Width * bitmap.Height) (fun _ -> 0uy)
    use memStream = new MemoryStream(pixels)
    use binWriter = new BinaryWriter(memStream)
    for y in 0..bitmap.Height-1 do
        for x in 0..bitmap.Width-1 do
            let px = bitmap.GetPixel(x,y)
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

let ConvertTexture spriteSheet inputFullName (job:Job) =
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
        use bitmap = new Bitmap(srcPath)
        let texconv = AppContext.BaseDirectory + "\\texconv.exe"
        let tmpout = FileInfo(job.OutputPath).DirectoryName
        sprintf "\"%s\" -ft DDS -f %s -o \"%s\"" (FileInfo(srcPath).FullName) ddsFormat tmpout
        |> Utils.StartWait texconv
        let head = {
            Format = TextureFormat.DDS
            Width = uint16 bitmap.Width
            Height = uint16 bitmap.Height
            SpriteSheet = spriteSheet
        }
        bitmap.Dispose()
        SaveTextureHead head dstWriter
        let ddsPath = 
            let baseFileName = 
                FileInfo(srcPath).Name
            tmpout + "\\" + baseFileName.[..baseFileName.LastIndexOf '.' - 1] + ".DDS"
        Utils.WaitForFile 5 ddsPath
        let ddsBytes = File.ReadAllBytes ddsPath
        dstWriter.Write (ddsBytes |> Array.length |> uint32)
        dstWriter.Write ddsBytes
        File.Delete ddsPath

    dstWriter.Close ()
    dstFile.Close ()

let Proc = {
    Proc = ConvertTexture Seq.empty false
    InputType = InputType.File
    Command = "ConvertTexture"
    FinishLogEnabled = true
    Prority = 100
}
