external toLocalTimeString: ReasonJs.Date.t => string = "" [@@bs.send "toLocalTimeString"];

module Clock = {
  module ClockSpec = {
    include ReactRe.Component.Stateful;
    let name = "Clock";

    type props = ();
    type state = {
      date: ReasonJs.Date.t
    };

    let getInitialState props => {
      date: ReasonJs.Date.make ()
    };

    let render {state} =>
      <div>
        <h1> (ReactRe.stringToElement "Hello, World") </h1>
        <h2> (ReactRe.stringToElement ("It is " ^ toLocalTimeString state.date)) </h2>
      </div>
  };

  include ReactRe.CreateComponent ClockSpec;
  let createElement ::children =>
    wrapProps () ::children;
};

let render () =>
  ReactDOMRe.render
    <Clock />
    (ReasonJs.Document.getElementById "root");

ReasonJs.setInterval render 1000;
