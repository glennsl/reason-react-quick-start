module Counter' = {
  /* Start */
  module Counter = {
    include ReactRe.Component.Stateful;
    let name = "Counter";

    type props = ();
    type state = {
      count: int
    };

    let getInitialState _ => {
      count: 0
    };

    let increment {state} _ =>
      Some { count: state.count + 1 };

    let render {state, updater} =>
      <div>
        <h1> (ReactRe.stringToElement ("Current count: " ^ string_of_int state.count)) </h1>
        <button onClick=(updater increment)>
          (ReactRe.stringToElement "Click to increment")
        </button>
      </div>;
  };
  /* End */

  include ReactRe.CreateComponent Counter;
  let createElement ::children =>
    wrapProps () ::children;
};

ReactDOMRe.renderToElementWithId <Counter' /> "root";
