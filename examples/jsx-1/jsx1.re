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
  <h1 title=(formatName user)>
    hello
  </h1>;

ReactDOMRe.renderToElementWithId header "root";
