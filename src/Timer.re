type audio = Dom.element;

[@bs.send] external play : audio => unit = "";

type running = {
  stopAt: Js.Date.t,
  timerId: Js.Global.intervalId,
  beepRef: ref(option(audio)),
};

let timeLeft = (stopAt) => ((stopAt |> Js.Date.valueOf) -. Js.Date.now());

let display = ({stopAt}) => {
  let toGoMillis = timeLeft(stopAt) |> int_of_float;
  let seconds = toGoMillis / 1000;
  let minutes = toGoMillis / 60 / 1000;
  string_of_int(minutes) ++ ":" ++ string_of_int(seconds);
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
  let setBeepRef = (theRef, {ReasonReact.state}) => {
    switch (state) {
    | Running(timer) => timer.beepRef := Js.Nullable.toOption(theRef)
    | _ => ()
    };
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
        }, 1000));
        self.send(Started({stopAt, timerId, beepRef: ref(None)})); 
      })
      | (Running({stopAt, timerId, beepRef}), End) =>
        ReasonReact.UpdateWithSideEffects(Waiting, self => {
          Js.Global.clearInterval(timerId);
          switch (beepRef^) {
          | Some(beep) => beep |> play
          | None => Js.log("No beep sound to play")
          };
        })
      | (Waiting, Started(timer)) => ReasonReact.Update(Running(timer))
      | (Running(timer), Tick) => {
        if (timeLeft(timer.stopAt) > 0.) {
          ReasonReact.Update(Running(timer))
        } else {
          ReasonReact.SideEffects(self => {
            self.send(End);
            switch (onEnd) {
            | Some(handler) => handler()
            | None => ()
            };
          });
        }
      }
      | _ => ReasonReact.NoUpdate
      },
    render: (self) => {
      let (display, cmd, buttonCollor, handler) =
        switch (self.state) {
        | Waiting => ("--:--", "Let's RUN!!!", "blue-darkest", start)
        | Running(timer) => (display(timer), "Stop it!", "pink", cancel)
        };
      <div className="w-1/2 sm:w-1/3 bg-blue-darkest rounded-lg shadow p-6
                      flex flex-col items-center items-stretch">
        <h1 className="self-center mb-4 text-5xl">
          (textEl(display))
        </h1>
        <button onClick=(self.handle(handler))
         className=("text-white p-2 rounded bg-" ++ buttonCollor ++
                    " border-blue-dark border-2 border-solid self-stretch")>
          (textEl(cmd))
        </button>

        <audio ref=(self.handle(setBeepRef)) src="../audio/beep.mp3"
               autoPlay=Js.Boolean.to_js_boolean(false) />
      </div>
    }
  }
};
