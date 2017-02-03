/* Part 1 */
module Welcome = {
  let createElement ::name ::children () =>
    <h1> (ReactRe.stringToElement ("Hello, " ^ name)) </h1>;
};

/* Part 2 */
ReactDOMRe.render
  <Welcome name="Bob" />
  (ReasonJs.Document.getElementById "root");
