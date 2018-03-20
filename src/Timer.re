type running = {
  stopAt: Js.Date.t,
  timerId: Js.Global.intervalId,
};

let timeLeft = (stopAt) => ((stopAt |> Js.Date.valueOf) -. Js.Date.now());

let twoDigitString = (number) => {
  let str = string_of_int(number);
  switch (String.length(str)) {
  | 1 => "0" ++ str
  | _ => str
  };
};

let display = ({stopAt}) => {
  let toGoMillis = timeLeft(stopAt) |> int_of_float;
  let seconds = (toGoMillis / 1000) mod 60;
  let minutes = toGoMillis / 60 / 1000;
  twoDigitString(minutes) ++ ":" ++ twoDigitString(seconds);
};

type state =
  | Waiting
  | Running(running)
  ;

type actions =
  | Start
  | Started(running)
  | Tick
  | End
  | Cancel
  ;

let component = ReasonReact.reducerComponent("Timer");

let textEl = ReasonReact.stringToElement;

let make = (~seconds, ~onStart=?, ~onEnd=?, ~onCancel=?, _children) => {
  let start = (event, self) => {
    switch (onStart) {
    | Some(handler) => handler(event)
    | None => ()
    };
    self.ReasonReact.send(Start);
  };
  let cancel = (event, self) => {
    switch (onCancel) {
    | Some(handler) => handler(event)
    | None => ()
    };
    self.ReasonReact.send(Cancel);
  };
  {
    ...component,
    initialState: () => Waiting,
    reducer: (action, state) =>
      switch (state, action) {
      | (Waiting, Start) => ReasonReact.SideEffects(self => {
        let stopAt = Js.Date.now() +. (float_of_int(seconds) *. 1000.)
                     |> Js.Date.fromFloat;
        let timerId = (Js.Global.setInterval(() => {
          self.send(Tick);
        }, 200));
        self.send(Started({stopAt, timerId})); 
      })
      | (Running({stopAt, timerId}), End) =>
        ReasonReact.UpdateWithSideEffects(Waiting, self => {
          Js.Global.clearInterval(timerId);
          switch (onEnd) {
          | Some(handler) => handler()
          | None => ()
          };
        })
      | (Waiting, Started(timer)) => ReasonReact.Update(Running(timer))
      | (Running(timer), Tick) =>
        if (timeLeft(timer.stopAt) > 0.) {
          ReasonReact.Update(Running(timer))
        } else {
          ReasonReact.SideEffects(self => self.send(End))
        }
      | _ => ReasonReact.NoUpdate
      },
    render: (self) => {
      let (display, cmd, digitsColor, btnColor, handler) =
        switch (self.state) {
        | Waiting => ("88:88", "Let's RUN!!!", "grey-darker", "blue-darkest", start)
        | Running(timer) => (display(timer), "Stop it!", "white", "pink", cancel)
        };
      <div className="w-3/5 sm:w-1/3 bg-blue-darkest rounded-lg shadow p-6
                      flex flex-col items-center items-stretch">
        <h1 className=("self-center mb-4 text-5xl font-mono text-" ++ digitsColor)>
          (textEl(display))
        </h1>
        <button onClick=(self.handle(handler))
         className=("text-white p-2 rounded bg-" ++ btnColor ++
                    " border-blue-dark border-2 border-solid self-stretch")>
          (textEl(cmd))
        </button>
      </div>
    }
  }
};
