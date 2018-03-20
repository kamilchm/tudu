type pomodoros = list(Pomodoro.pomodoro([`Completed]));

type submitting = {
  pomodoro: Pomodoro.pomodoro([`Started]),
  descriptionField: ref(option(Dom.element)),
};

type audio = Dom.element;

[@bs.send] external play : audio => unit = "";

/* https://github.com/excitement-engineer/Red/blob/c34c88b2bf6556bb31df54eb23b13c863cc6460d/src/notification.re */

type working = {
  pomodoro: Pomodoro.pomodoro([`Started]),
  beepRef: ref(option(audio)),
};

type state =
  | Waiting(pomodoros)
  | Running(working, pomodoros)
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

let setBeepRef = (theRef, {ReasonReact.state}) =>
  switch (state) {
  | Running(s, _) => s.beepRef := Js.Nullable.toOption(theRef)
  | _ => ()
  };

let update = (action, state) =>
  switch (state, action) {
  | (Waiting(state), TimerStart) => ReasonReact.Update(
    Running({pomodoro: Pomodoro.start(Js.Date.now()), beepRef: ref(None)}, state))
  | (Running({pomodoro, beepRef}, state), TimerEnd) => ReasonReact.UpdateWithSideEffects(
      Submitting({pomodoro, descriptionField: ref(None)}, state),
      self => switch (beepRef^) {
      | Some(beep) => beep |> play
      | None => Js.log("No beep sound to play")
      }
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
  let beepAudio =
    <audio ref=(handle(setBeepRef)) src="/audio/beep.mp3"
           autoPlay=Js.Boolean.to_js_boolean(true) />;

  switch(state) {
  | Waiting(_) =>
    <div className=(containerClasses)>
      timer
      (msg("Timer is WAITING..."))
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
      beepAudio
    </div>
  }
};

let make = (_children) => {
  ...component,
  initialState: () => Waiting([]),
  reducer: update,
  render: view({pomoTime: 25 * 60})
};
