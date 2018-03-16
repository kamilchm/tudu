type pomodoros = list(Pomodoro.any_pomodoro);

type state =
  | Waiting(pomodoros)
  | Running(pomodoros)
  ;

type actions =
  | TimerStart
  | TimerEnd
  | TimerCancel
  ;

let component = ReasonReact.reducerComponent("Pomodoros");

let textEl = ReasonReact.stringToElement;

let make = (_children) => {
  ...component,
  initialState: () => Waiting([]),
  reducer: (action, state) =>
    switch (state, action) {
    | (Waiting(state), TimerStart) => ReasonReact.Update(Running(state))
    | (Running(state), TimerEnd) => ReasonReact.Update(Waiting(state))
    | _ => ReasonReact.NoUpdate
    },
  render: (self) => {
    let msg =
      switch (self.state) {
      | Waiting(_) => "Timer is WAITING..."
      | Running(_) => "Timer is RUNNING!!!"
      };
    <div className="container bg-blue-darker h-screen text-white
                    flex flex-col justify-center items-center">
        <Timer seconds=10
               onStart=(_ev => self.send(TimerStart))
               onEnd=(_ev => self.send(TimerEnd))/>
        <div className="m-4">
          (textEl(msg))
        </div>
    </div>
  }
};
