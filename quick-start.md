
# Rehydrate quick start

## Introduction

__rehydrate is still in stealth mode, getting some polish before the official announcement. Please don't publicize this. In the meantime, come join us in [Discord!](https://discord.gg/reasonml)__

The next few sections will gradually introduce you to using rehydrate. We will gradually introduce new concepts and show more complex examples. Once you master these, you should be ready to go!

You can also clone this repo and build the examples yourself, to see the full context and how they transpile to js.

## Prerequisites

rehydrate is a library built on React and best used with Reason. You are expected to know the basics of both Reason and React. If you don't feel very confident, we recomend refreshing your knowledge so you can follow along more easily:
* [An introduction to Reason](https://kennetpostigo.gitbooks.io/an-introduction-to-reason/content/)
* [React Quick Start](https://facebook.github.io/react/docs/hello-world.html)

## Additional resources:
* [rehydrate documentation proper](https://github.com/reasonml/rehydrate/blob/master/documentation.md)

## Installing


## Hello world!

The smallest rehydrate example looks like this:

```reason
ReactDOMRe.render
  <h1> (ReactRe.stringToElement "Hello, world!") </h1>
  (ReasonJs.Document.getElementById "root");
```

Not too scary is it! This will render a header saying "Hello World" on the page. Let's move on.


## Yep, there's JSX!

Reason has excellent support for JSX syntax, but there are some differences to be aware of:


### No curly braces around embedded expressions

Reason's JSX does not need curly braces to embed expressions. Though complex expressions need to be surrounded by parentheses. And in Reason most everything is an expression, so there is barely any limit to what you can intermingle in your JSX.

```reason
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
```


### JSX is typechecked too

As in JSX for JavaScript, JSX in Reason is really just syntax sugar for normal Reason code. And because of this your JSX will typechekd too. If you mistype an attribute the typechecker will give you a nudge. As it also will if you pass a `boolean` to an attribute that expects a `string`.

But this also means `null` and `string`s or even `list`s and `array`s aren't valid types for expressions embedded in JSX. Components expect their children to be React elements. And so because of this we need to wrap these in React elements. Luckily rehydrate provides functions to do exactly that:

* `ReactRe.stringToElement` will wrap a string
* `ReactRe.arrayToElement` will wrap an array of React elements
* `ReactRe.nullElement` doesn't wrap anything, but will behave like a null in React.js

```reason
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

ReactDOMRe.render
  list
  (ReasonJs.Document.getElementById "root");
```

## Components

Components require that we dabble a bit in the black arts. But not too much. A basic component looks like this

```reason
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
```

So we define a module, which is really just a specification. `include ReactRe.Component` is the bit of black magic that tells rehydrate what kind of component it is, and what it should expect to see defined in it. `ReactRe.Component` is the most basic kind, and requires us to define a `name` a `props` type, and a `render` function. `name` is the component's display name, which is useful for debugging. `props` specifies the type of the component's attributes/properties. And `render` is, as you'd expect, the function that's called when the component is to be rendered.

The actual component is defined by the second piece of black magic: `include ReactRe.CreateComponent Welcome`. Under the hood this will create a `comp` variable, which is the actual component and, as we'll learn later, is important for js interop. It will also create a `wrapProps` function, which we then use to define the `createElement` function, which really just translates our properties from a labeled argument form (`::name`) into the form of a record that fits our `props` type.

### Look, ma, no magic!

As far as Reason's JSX syntax is concerned, a component is nothing more than a module with a createElement function of a certain shape. So if we remove all the magic (and all the convenient functionality) we can define a component like this:

```reason
module Welcome = {
  let createElement ::name ::children () =>
    <h1> (ReactRe.stringToElement ("Hello, " ^ name)) </h1>;
};
```

This is significanlty less code, and for really small components this is sometimes convenient, but most often you'll want the magical convenience. Both components can be used like this:

```reason
ReactDOMRe.render
  <Welcome name="Bob" />
  (ReasonJs.Document.getElementById "root");
```

### Stateful components

#### Updating state

### Events

### Lifecycle hooks

## Javascript interop

## Community

[Join us in Discord!](https://discord.gg/reasonml)

## Advanced topics

### Desugaring of uncapitalized JSX

As you plod along you might start wondering how JSX handles the out-of-the-box HTML elements, or uncapitalized components more generally. If you're really curious you might have noticed that `<div foo=bar>child1 child2</div>` desugars `div foo::bar children::[child1, child2] () [@JSX]`. So where's the `div` function? It actually doesn't exist. The `[@JSX]` annotation invokes some even more arcane black magic, a ppx transform that translates that into `ReactDOMRe.createElement "div" props::(ReactDOMRe.props foo::bar ()) [|child1, child2|];`

So why is the middle step even there? I suspect it's in order to decouple the JSX from rehydrate and its ReactDOMRe module. Not sure how you'd avoid the ppx in practice though... [TODO: explain better]


### High-order components and other patterns
