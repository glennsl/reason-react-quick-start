let items = [
  "apples",
  "pears",
  "cashews",
  "pecans"
];

let isNuts = fun
| "cashews"
| "pecans" => true
| _ => false;

let nutIcon =
  <i className="icon-nut" />;

let item name =>
  <li>
    (ReactRe.stringToElement name)
    (isNuts name ? nutIcon : ReactRe.nullElement)
  </li>;

let list =
  <ul>
    (items |> List.map item |> Array.of_list |> ReactRe.arrayToElement)
  </ul>;

ReactDOMRe.renderToElementWithId list "root";
