open Tea
open Pomodoro

type state = { pomodoros: pomodoro list }

type model =
  | Waiting of state
  | Running of state

let model = Waiting { pomodoros = [] }

type msg =
  | TimerStart
  | TimerStartAt of float
  | TimerTick
  | TimerStop
[@@bs.deriving {accessors}]

let update model msg =
  match (model, msg) with
  | (Waiting state, TimerStart) -> Running state, Cmd.none
  | (Running state, TimerStop) -> Waiting state, Cmd.none
  | _ -> assert false

let view model =
  let open Html in
  div []
    [ p [] [ Printf.sprintf "Timer is %s"
      (match model with | Waiting _ -> "WAITING..."
                        | Running _ -> "RUNNING")
             |> text
           ]
    ; p [] [ button
              [ onClick (match model with | Waiting _ -> TimerStart
                                          | Running _ -> TimerStop)
              ]
              [ text (match model with | Waiting _ -> "Start"
                                       | Running _ -> "Stop")
              ]
           ]
    ]

let init () =
  model, Cmd.none

let main =
  App.standardProgram
    { init
    ; update
    ; view
    ; subscriptions = (fun _ -> Sub.none)
    }
