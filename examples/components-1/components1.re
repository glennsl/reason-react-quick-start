module Welcome = {
  include ReactRe.Component;
  let name = "Welcome";

  type props = {
    name: string
  };

  let render {props} =>
    <h1> (ReactRe.stringToElement ("Hello, " ^ props.name)) </h1>;
};

include ReactRe.CreateComponent Welcome;
let createElement ::name ::children =>
  wrapProps { name } ::children;
