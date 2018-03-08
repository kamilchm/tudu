type tags = string list

type started = { startDate: float; }
type completed = { startDate: float; endDate: float; details: string; tags: tags }
type cancelled = { startDate: float; endDate: float; details: string option; tags: tags }

type pomodoro = [
  | `Started of started
  | `Completed of completed
  | `Cancelled of cancelled
]

let start time = `Started { startDate = time }

let complete endDate details tags (`Started (p: started)): [> `Completed of completed ] =
  `Completed { startDate = p.startDate; endDate; details; tags; }

let cancel endDate details tags (`Started (p: started)): [> `Cancelled of cancelled ] =
  `Cancelled { startDate = p.startDate; endDate; details; tags}
