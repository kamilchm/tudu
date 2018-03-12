type tags = string list

type started = { startDate: float; }
type completed = { startDate: float; endDate: float; details: string; tags: tags }
type cancelled = { startDate: float; endDate: float; details: string option; tags: tags }

type _ pomodoro =
  | Started : started -> [`Started] pomodoro
  | Completed : completed -> [`Completed] pomodoro
  | Cancelled : cancelled -> [`Cancelled] pomodoro

let start time = Started { startDate = time }

let complete endDate details tags = function
  | Started p -> Completed { startDate = p.startDate; endDate; details; tags }

let cancel endDate details tags = function
  | Started p -> Cancelled { startDate = p.startDate; endDate; details; tags }

type any_pomodoro = Any_pomodoro : 'a pomodoro -> any_pomodoro

