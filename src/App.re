open Belt;

type pomodoros = list(Pomodoro.pomodoro([`Completed]));

type submitting = {
  pomodoro: Pomodoro.pomodoro([`Started]),
  descriptionField: ref(option(Dom.element)),
};

type state =
  | Waiting(pomodoros)
  | Running(Pomodoro.pomodoro([`Started]), pomodoros)
  | Submitting(submitting, pomodoros)
  ;

type actions =
  | TimerStart
  | TimerEnd
  | TimerCancel
  | Submit
  ;

type config = {
  pomoTime: int,
};

let component = ReasonReact.reducerComponent("App");
 
let setDescriptionFieldRef = (theRef, {ReasonReact.state}) =>
  switch (state) {
  | Submitting(s, _) => s.descriptionField := Js.Nullable.toOption(theRef)
  | _ => ()
  };

let update = (action, state) =>
  switch (state, action) {
  | (Waiting(state), TimerStart) => ReasonReact.Update(
    Running(Pomodoro.start(Js.Date.now()), state))
  | (Running(pomodoro, state), TimerEnd) => ReasonReact.UpdateWithSideEffects(
      Submitting({pomodoro, descriptionField: ref(None)}, state),
      (_) => Alarm.fire(~message="Time for a break!", ~title="Time's up")
    )
  | (Submitting(s, pomodoros), Submit) =>
    switch (s.descriptionField^) {
    | Some(df) => { 
      let dn = ReactDOMRe.domElementToObj(df);
      ReasonReact.Update(Waiting(
          [Pomodoro.complete(Js.Date.now(), dn##value, [], s.pomodoro), ...pomodoros]))
    }
    | None => ReasonReact.NoUpdate
    }
  | _ => ReasonReact.NoUpdate
  };

let textEl = ReasonReact.stringToElement;

let view = (config, {ReasonReact.state, handle, send}) => {
  let timer =
    <Timer seconds=(config.pomoTime)
           onStart=(_ev => send(TimerStart))
           onEnd=(_ev => send(TimerEnd))/>;
  let msg = (text) =>
    <div className="m-4">
      (textEl(text))
    </div>;
  let containerClasses = "container bg-blue-darker h-screen text-white
                          flex flex-col justify-center items-center";
  
  let pomodoItem = (key, details) =>
    <div key=(key)>(textEl(details))</div>;

  switch(state) {
  | Waiting(pomos) =>
    <div className=(containerClasses)>
      timer
      (msg("Timer is WAITING..."))
      (pomos
      |> List.mapWithIndex(_, (i, t) => {
        let key = string_of_int(i);
        switch (t) {
        | Pomodoro.Completed({details}) => details |> pomodoItem(key)
        | _ => "Unknown pomodoro type" |> pomodoItem(key)
        };
      })
      |> List.toArray
      |> ReasonReact.arrayToElement
      )
    </div>
  | Running(_) =>
    <div className=(containerClasses)>
      timer
      (msg("Timer is RUNNING!!!"))
    </div>
  | Submitting(_) =>
    <div className=(containerClasses)>
      <div className="flex flex-col items-stretch items-center">
        <h1>(textEl("What did you do?"))</h1>
        <input ref=(handle(setDescriptionFieldRef))
          _type="text" placeholder="something interesting..."
          className="bg-transparent mt-4 mb-4 text-2xl text-white
                     border-b border-b-2 border-pink">
        </input>
        <button onClick=((_) => send(Submit))
          className="text-white p-2 rounded bg-pink
                    border-blue-dark border-2 border-solid self-stretch">
          (textEl("Submit"))
        </button>
      </div>
    </div>
  }
};

let make = (_children) => {
  ...component,
  initialState: () => Waiting([]),
  didMount: (_) => {
    Alarm.register();
    ReasonReact.NoUpdate
  },
  reducer: update,
  render: view({pomoTime: 5})
};
