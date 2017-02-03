type user = { firstName: string, lastName: string };

let user: user = {
  firstName: "Harper",
  lastName: "Perez"
};

let formatName user =>
  user.firstName ^ " " ^ user.lastName;

let hello =
  (ReactRe.stringToElement ("Hello, " ^ formatName user));

let header =
  <h1 role=(formatName user)> /* TODO: role should be title */
    hello
  </h1>;

ReactDOMRe.render
  header
  (ReasonJs.Document.getElementById "root");
