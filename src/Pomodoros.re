type pomodoros = list(Pomodoro.any_pomodoro);

type state =
  | Waiting(pomodoros)
  | Running(pomodoros)
  ;

type actions =
  | TimerStart
  | TimerStartAt(float)
  | TimerTick
  | TimerStop
  ;

let component = ReasonReact.reducerComponent("Pomodoros");

let textEl = ReasonReact.stringToElement;

let make = (_children) => {
  ...component,
  initialState: () => Waiting([]),
  reducer: (action, state) =>
    switch (state, action) {
    | (Waiting(state), TimerStart) => ReasonReact.Update(Running(state))
    | (Running(state), TimerStop) => ReasonReact.Update(Waiting(state))
    | _ => ReasonReact.NoUpdate
    },
  render: (self) => {
    let msg =
      switch (self.state) {
      | Waiting(_) => "Timer is WAITING..."
      | Running(_) => "Timer is RUNNING!!!"
      };
    let (cmd, action) =
      switch (self.state) {
      | Waiting(_) => ("Let's RUN!!!", TimerStart)
      | Running(_) => ("Stop it!", TimerStop)
      };
    <div className="app container">
      <div className="timer">
        <div>
          (textEl(msg))
        </div>
        <div>
          <button onClick=(_event => self.send(action))>
            (textEl(cmd))
          </button>
        </div>
      </div>
    </div>
  }
};
