module Bake.Snowing.Encrypt

open Bake

open System.Diagnostics
open System.IO
open System.Threading
open System
open System.Security.Cryptography

let mutable private aes : System.Security.Cryptography.AesCryptoServiceProvider = null

let EncryptFile src dst = 
    src
    |> File.ReadAllBytes
    |> fun fileBytes ->
        let outputContent = Array.copy fileBytes
        aes.CreateEncryptor().TransformBlock(fileBytes,0,fileBytes.Length/16*16,outputContent,0)
        |> ignore
        File.WriteAllBytes(dst,outputContent)

[<BakeAction>]
let SetAESKey = {
    help = "设置AES密钥文件"
    usage = []
    example = []
    action = fun ctx script ->
        Utils.verifyArgumentCount script 1
        let key,iv = 
            "AesKey.bin"
            |> File.ReadAllBytes
            |> Array.splitAt 16
        aes <- new System.Security.Cryptography.AesCryptoServiceProvider ()
        aes.KeySize <- 128
        aes.BlockSize <- 128
        aes.Key <- key
        aes.IV <- iv
        
        Seq.empty, ctx
}

[<BakeAction>]
let Encrypt = {
    help = "对文件进行AES加密"
    usage = []
    example = []
    action = fun ctx script ->
        Utils.verifyArgumentCount script 3
        let dstDir = script.arguments.[0] |> Utils.applyContextToArgument ctx |> Utils.normalizeDirPath
        let ext = script.arguments.[1] |> Utils.applyContextToArgument ctx
        let inputFile = script.arguments.[2] |> Utils.applyContextToArgument ctx
        Utils.blockArgumentTaskPerLine (fun ctx script line ->
            Utils.matchInputFiles script.scriptFile.DirectoryName line
            |> Seq.map (fun (path, fileName) ->
                let dst = dstDir + fileName |> Utils.modifyExtensionName ext
                {
                    inputFiles = seq { FileInfo path }
                    source = script
                    outputFiles = seq { dst }
                    dirty = false
                    run = fun () -> 
                        lock stdout (fun () -> printfn "Encrypt:%s..." fileName)
                        EncryptFile path dst
                })) ctx script inputFile
        , ctx
}

let wrapToEncryptAction (action: BakeAction) =
    { action with
        help = "(AES加密)"
        action = fun ctx script ->
            let tasks, ctx = action.action ctx script
            let tasks = 
                tasks
                |> Seq.map (fun task ->
                    { task with
                        run = fun () ->
                            task.run ()
                            task.outputFiles |> Seq.toArray |> Array.Parallel.iter (fun x -> EncryptFile x x) } ) 
            tasks, ctx }
