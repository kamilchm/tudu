module JsAudio = {
  type audio;

  [@bs.new] external make : string => audio = "Audio";

  [@bs.send] external play : audio => unit = "play";
};

module JsNotification = {
  type notification;

  type content = {. "body": string, "icon": string };

  [@bs.new] external make : (string, content) => notification = "Notification";

  [@bs.val] external requestPermission : unit => unit = "Notification.requestPermission";
};

[@bs.module] external alarmSound : string = "../../../src/audio/beep.mp3";

let register = () =>
  try (JsNotification.requestPermission()) {
  | _ => Js.log("Notifications not supported.")
  };

let fire = (~title, ~message) => {
  let content = {"body": message, "icon": "https://red.now.sh/favicon.ico"};
  let notify = () => {
    ignore(JsNotification.make(title, content));
    JsAudio.( make(alarmSound) |> play )
  };
  try (notify()) {
  | _ => Js.log("Error sending notification.")
  }
};
