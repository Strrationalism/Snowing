module BinPack

type InputRectangle<'Tag> = 
    {
        W: int
        H: int
        Tag: 'Tag
    }

[<Struct>]
type Rectangle = 
    {
        X: int
        Y: int
        W: int
        H: int
    }

[<Struct>]
type Space = Space of Rectangle

type Result<'Tag> = 
    {
        Rects: (Rectangle * 'Tag) list
        Spaces: Space list
        Width: int
        Height: int
    }

let private defaultResult =
    {
        Rects = []
        Spaces = []
        Width = 0
        Height = 0
    }

let tryPutRectIntoSpace (rect: InputRectangle<'a>) (Space space) =
    let tag = rect.Tag
    let rect = 
        {
            X = space.X
            Y = space.Y
            W = rect.W
            H = rect.H
        }
    let space11 = 
        {
            space with
                X = space.X + rect.W
                W = space.W - rect.W
        }
    let space12 =
        {
            space with
                Y = space.Y + rect.H
                H = space.H - rect.H
                W = rect.W
        }
    let space21 = 
        {
            space with
                Y = space.Y + rect.H
                H = space.H - rect.H
        }
    let space22 =
        {
            space with
                X = space.X + rect.W
                W = space.W - rect.W
                H = rect.H
        }
    let space01, space02 =
        seq { space11, space12 ; space21, space22 }
        |> Seq.minBy (fun (x, _) -> abs (x.W - x.H))

    let valid =
        seq { space01; space02 }
        |> Seq.tryFind (fun x -> x.W < 0 || x.H < 0)
        |> function
            | Some _ -> false
            | None -> true
    if not valid then None
    else 
        let spaces =
            [ space01; space02 ]
            |> List.filter (fun x -> x.W <> 0 && x.H <> 0)
            |> List.map Space
        Some (rect, spaces, tag, Space space)
    

let rec private binPackInner prevResult input =
    match input with
        | [] -> prevResult
        | current :: tail ->
            let putResults =
                prevResult.Spaces
                |> List.choose (tryPutRectIntoSpace current)
            match putResults with
                | [] ->
                    let result =
                        if current.W + prevResult.Width > current.H + prevResult.Height then
                            let newWidth = max prevResult.Width current.W
                            { 
                                prevResult with 
                                    Width = newWidth
                                    Height = prevResult.Height + current.H 
                                    Spaces = 
                                        let space =
                                            Space {
                                                X = 0
                                                Y = prevResult.Height
                                                W = newWidth
                                                H = current.H
                                            }
                                        space :: prevResult.Spaces
                            }
                        else 
                            let newHeight = max prevResult.Height current.H
                            { 
                                prevResult with 
                                    Width = prevResult.Width + current.W 
                                    Height = newHeight
                                    Spaces =
                                        let space =
                                            Space {
                                                X = prevResult.Width
                                                Y = 0
                                                W = current.W
                                                H = newHeight
                                            }
                                        space :: prevResult.Spaces
                            }
                    binPackInner result (current::tail)
                | putResults ->
                    let rect, spaces, tag, originSpace =
                        putResults
                        |> List.minBy (fun (_, spaces, _, _) -> 
                            spaces
                            |> List.sumBy (fun (Space a) -> a.W * a.H))
                    let result = 
                        {
                            prevResult with
                                Rects = (rect, tag) :: prevResult.Rects
                                Spaces = 
                                    spaces @ (prevResult.Spaces |> List.except [ originSpace ])
                        }
                    binPackInner result tail
                        
let binPack<'a> (input: InputRectangle<'a> list) =
    [
        fun (x: InputRectangle<'a> list) -> x
        List.sortByDescending (fun x -> x.W)
        List.sortByDescending (fun x -> x.W * x.H)
        List.sortByDescending (fun x -> x.H)
        List.sortByDescending (fun x -> x.W + x.H)
        List.sortByDescending (fun x -> max x.W x.H)
    ]
    |> List.map (fun f -> binPackInner defaultResult <| f input)
    |> List.maxBy (fun r -> 
        let allArea = r.Width * r.Height
        let rectArea = r.Rects |> List.sumBy (fun (x, _) -> x.W * x.H)
        float rectArea / float allArea)
