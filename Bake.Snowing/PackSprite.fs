module JobProcs.PackSprite

open System.IO

open Bake
open Bake.Snowing
open Bake.Snowing.ConvertTexture
open BinPack
open SixLabors.ImageSharp
open SixLabors.ImageSharp.PixelFormats
open SixLabors.ImageSharp.Processing

let private spriteLock = obj()

let private PackSpriteFunc (job:Job) =
    let inputDir = job.ScriptDir.FullName + "\\" + job.Input.Head
    let sprites =
        Directory.EnumerateFiles inputDir
        |> Seq.map (fun x -> 
            let x = FileInfo x
            let id = x.Name.[..(-1 + x.Name.IndexOf '.')] |> uint32
            let image = Image.Load<Rgba32> x.FullName
            { W = image.Width + 2; H = image.Height + 2; Tag = id, image })
        |> Seq.toList
        |> binPack

    use mergedTexture = 
        let wrapTo4 a = 
            let x = a % 4
            if x > 0 then (4 - x) + a
            else a
        let w, h = wrapTo4 sprites.Width, wrapTo4 sprites.Height
        let buffer = Array.init (w * h) (fun _ -> Rgba32 0u)
        Image.WrapMemory<Rgba32>(System.Memory(buffer), w, h)

    let mergedPath = job.OutputPath + ".png"
    sprites.Rects
    |> List.iter (fun (rect, (id, image)) ->
        mergedTexture.Mutate (fun x -> 
            x.DrawImage(image, new Point(rect.X + 1, rect.Y + 1), PixelColorBlendingMode.Normal, 1.0f)
            |> ignore))

    let spriteFrames =
        let maxFrameID =
            sprites.Rects
            |> List.maxBy (fun (rect,(id, _)) -> id)
            |> snd |> fst
        let ret =
            Array.init (maxFrameID |> int |> (+) 1) (fun _ -> { x = 0us; y = 0us; w = 0us;h = 0us })
        sprites.Rects
        |> List.iter (fun (frame,(id,_)) ->
            ret.[int id] <- 
                { x = uint16 (frame.X + 1); y = uint16 (frame.Y + 1); w = uint16 (frame.W - 2); h = uint16 (frame.H - 2) })
        ret |> Array.toList
        

    mergedTexture.SaveAsPng mergedPath

    let newJob = {
        Input = [ job.OutputPath + ".png" ]
        OutputPath = job.OutputPath
        Arguments = job.Arguments
        ScriptDir = job.ScriptDir
    }
    
    ConvertTextureFunc spriteFrames true newJob
    File.Delete mergedPath


[<BakeAction>]
let PackSprite = {
    help = "从图片文件夹构建精灵文件"
    usage = []
    example = []
    action = fun ctx script ->
        let out = script.arguments.[0] |> Utils.applyContextToArgument ctx
        let input = script.arguments |> List.last |> Utils.applyContextToArgument ctx 
        let options = script.arguments.[1..script.arguments.Length-2]

        seq { {
            dirty = false
            source = script
            inputFiles = Utils.normalizeDirPath script.scriptFile.Directory.FullName + input |> Directory.EnumerateFiles |> Seq.map FileInfo
            outputFiles = seq { out }
            run = fun () ->
                lock stdout (fun () -> printfn "PackSprite:%s..." input)
                PackSpriteFunc {
                    Arguments = options
                    Input = [ input ]
                    ScriptDir = script.scriptFile.Directory
                    OutputPath = FileInfo(out).FullName
                }
        } },
        ctx

}


[<BakeAction>]
let ``PackSprite-Encrypt`` = Encrypt.wrapToEncryptAction PackSprite
