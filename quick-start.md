
# Reason-React quick start

## Introduction

Reason-React is a pretty thin layer of reason/ocaml bindings to React.

The next few sections will gradually introduce you to using Reason-React. We will gradually introduce new concepts and show more complex examples. Once you master these, you should be ready to go!

You can also clone this repo and build the examples yourself, to see the full context and how they transpile to js.

### Prerequisites and additional resources

Reason-React is a library built on React and BuckleScript and best used with Reason. You are expected to know the basics of both Reason and React. If you don't feel very confident, we recommend refreshing your knowledge so you can follow along more easily:
* [An introduction to Reason](https://kennetpostigo.gitbooks.io/an-introduction-to-reason/content/)
* [React Quick Start](https://facebook.github.io/react/docs/hello-world.html)

You are not expected to know much about BuckleScript for the purpose of this guide alone, other than the fact that it somehow, as if by magic, converts OCaml/Reason code to Javascript. But you will eventually want to refer to the BuckleScript documentation, for configuration of `bsb`, js interop and such. And you'll want to refer to the Reason-React documentation for the gritty details:
* [BuckleScript User Manual](https://bloomberg.github.io/bucklescript/Manual.html)
* [Reason-React documentation proper](https://github.com/reasonml/reason-react/blob/master/documentation.md)

## Getting started

A good place to start is by cloning and setting up `reason-react-example`:

```sh
git clone https://github.com/chenglou/reason-react-example.git
cd reason-react-example
npm install
npm start
```

Then in a different terminal do:
```sh
npm run build
```

`npm start` will run the bucklescript build system (`bsb`) in watch mode. `npm run build` runs `webpack` in watch mode, which will pick up the artifacts produced by `bsb` and bundle them together with its dependencies to produce a single javasrcipt file (for each configured target).

The example project has `bsb` configured to watch the `src` folder, and webpack configured to pick up the artifact produced by bsb from `index.re`, and output `index.js` in the `src` folder alongside `index.html`, which can then be used to run it in a web browser. To start with you will most likely want to use the existing configuration with `index.re` as your entrypoint. You can still split your code into multiple source files. As long as they are alongside and used by `index.re` they'll get picked up by `bsb` and `webpack`.

## A small step for... oh, "hello world"

The smallest Reason-React example looks like this:

[//]: # "hello-world1"
```reason
ReactDOMRe.render
  <h1> (ReactRe.stringToElement "Hello, world!") </h1>
  (ReasonJs.Document.getElementById "root");
```

Not too scary is it! This will render a header saying "Hello World" on the page. Ok, thnx, let's move on...


## There's JSX? There's JSX!

Reason has excellent support for the JSX syntax, but there are some differences to be aware of:


### No curly braces around embedded expressions

Reason's JSX does not need curly braces to embed expressions. Though complex expressions need to be surrounded by parentheses. And in Reason most everything is an expression, so there is barely any limit to what you can intermingle in your JSX. Also, you _can_ surround expressions by curly braces if you really want to, you just don't need to. Surrounding something in curly braces anywhere will merely introduce a new lexical scope.

[//]: # "jsx1"
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
  <h1 title=(formatName user)>
    hello
  </h1>;

ReactDOMRe.render
  header
  (ReasonJs.Document.getElementById "root");
```


### The JSX is typechecked

As with the babel JSX  transform for JavaScript, Reason's JSX is really just syntax sugar for plain Reason code. And because of this your JSX will typechecked too. If you mistype an attribute the typechecker will give you a nudge, as it also will if you pass a `boolean` to an attribute that expects a `string`.

But this also means `null`s and `string`s or even `list`s and `array`s aren't valid types for expressions embedded in JSX. Components expect their children to be React elements, and so we need to wrap them as elements. Luckily Reason-React provides functions to do exactly that:

* `ReactRe.stringToElement` will wrap a string
* `ReactRe.arrayToElement` will wrap an array of React elements
* `ReactRe.nullElement` will pretend to be an element for the typechecker, but is really just a null

[//]: # "jsx2"
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

Components require that we dabble a bit in the black arts. But just a little bit. A basic component looks like this

[//]: # "components1"
```reason
module Welcome = {
  include ReactRe.Component;
  let name = "Welcome";

  type props = {
    name: string
  };

  let render { props } =>
    <h1> (ReactRe.stringToElement ("Hello, " ^ props.name)) </h1>;
};

include ReactRe.CreateComponent Welcome;
let createElement ::name ::children =>
  wrapProps { name } ::children;
```

So we define a module, which isn't and won't be the actual component but it's specification. `include ReactRe.Component` is the bit of black magic that tells Reason-React what kind of component it is, and what it should expect to see defined in it. `ReactRe.Component` is the most basic kind, and requires us to define a `name` a type called `props`, and a `render` function.
* `name` is the component's display name, which is useful for debugging.
* `props` specifies the type of the component's attributes/properties.
* `render` is, as you'd expect, the function that's called when the component is to be rendered.

The actual component is defined by the second piece of black magic: `include ReactRe.CreateComponent Welcome`. Under the hood this will create a `comp` variable, which is the actual component and, as we'll learn later, is important for js interop. It will also create a `wrapProps` function, which we then use to define the `createElement` function, and which really just translates our properties from a labeled argument form (`::name`) into the form of a record that fits our `props` type.

### The elusive "component bag"

Since idiomatic Reason/OCaml code does not use `this`, Reason-React defines a data structure that's called the "component bag" which fulfills the same role. Which is to avoid having to specify every damn thing you might want to use, just in case. Therefore many of the functions you define on your components, like the `render` function, will get passed a "component bag". It is a record of the shape `{props, state, updater, refSetter, instanceVars, setState}`, and you'll usually want to destructure the argument directly. If you need access to the state, you just add that like so: `let render { props, state } => ...`

### Look, ma, no magic!

As far as Reason's JSX syntax is concerned, a component is nothing more than a module with a createElement function of a certain shape. So if we remove all the magic (and all the convenient functionality) we can define a component like this:

[//]: # "components2 - Part 1"
```reason
module Welcome = {
  let createElement ::name ::children () =>
    <h1> (ReactRe.stringToElement ("Hello, " ^ name)) </h1>;
};
```

This is significanlty less code, and for tiny components this is sometimes a convenient trick, but most often you'll want all the magical convenience. Both components can be used like this:

[//]: # "components2 - Part 2"
```reason
ReactDOMRe.render
  <Welcome name="Bob" />
  (ReasonJs.Document.getElementById "root");
```

### Stateful components

With the magic back in, adding local state to a component is as easy telling Reason-React this is a stateful component, what the type of our state is, and what the initial state is:

[//]: # "components3"
```reason
module Clock = {
  module ClockSpec = {
    include ReactRe.Component.Stateful;
    let name = "Clock";

    type props = ();
    type state = {
      date: Js.Date.t
    };

    let getInitialState _ => {
      date: Js.Date.make ()
    };

    let render {state} =>
      <div>
        <h1> (ReactRe.stringToElement "Hello, World") </h1>
        <h2> (ReactRe.stringToElement ("It is " ^ Js.Date.toLocaleTimeString state.date)) </h2>
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
```

Compared to our earlier component, not much fundamental has changed. `include ReactRe.Component` is now `include ReactRe.Component.Stateful`, we've defined a `state` type and a `getInitialState` function which takes `props` as its argument and returns the initial state.

There's not much use in state if it doesn't change, though. So let's get to that.

#### Updating state

Reason-React does have a setState function, but you shouldn't normally use it! Instead you'll usually use the `updater` function that's passed to you in the component bag. Let's make a counter, where the count increases on each click of a button.

[//]: # "components3"
```reason
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
```

Here we get the `updater` function from the "component bag" passed to our `render` function, which we use to wrap our `increment` function before setting it as the button's `onClick` handler. When the button is clicked, `increment` will be called with the latest state (and whatever else is in the "component bag"). It then returns the new state, which will cause the component to rerender with the new state.

Note that `updater` expects a function that returns `option state`, where returning None avoids the rerender because no state has changed. Also beware that `updater` will return a function that only takes unit. If you don't pass it on to a handler and intend to execute it, don't forget to actually do it. There's little syntactic difference between function application and partial application, and the compiler might not warn about it (see https://github.com/reasonml/reason-react/issues/36). Compare these:

```reason
let f : string => unit = fun value =>
  updater (fun { state } () => Some { ...state, value }) ();
```
```reason
let f : string => unit = fun value =>
  updater (fun { state } () => Some { ...state, value });
```

The `setState` function differs from `updater` in that the latter is bound to the state a component has when `updater` gets created, while setState will always give you the fresh state. The downside of `setState` is that it must return a new state, and will cause a rerender even if nothing changed. This is due to a shortcoming with `setState` in React proper, and hopefully a very temporary problem (see https://github.com/reasonml/reason-react/issues/37).

### Lifecycle hooks

### Events

## Javascript interop

## Community

We gather around the bonfire on the reasonml discord. [You should come join us!](https://discord.gg/reasonml)

## Advanced topics

### Desugaring of uncapitalized JSX

As you plod along you might start wondering how JSX handles the out-of-the-box HTML elements, or uncapitalized components more generally. If you're really curious you might have noticed that `<div foo=bar>child1 child2</div>` desugars `div foo::bar children::[child1, child2] () [@JSX]`. So where's the `div` function? It actually doesn't exist. The `[@JSX]` annotation invokes some even more arcane black magic, a ppx transform that translates that into `ReactDOMRe.createElement "div" props::(ReactDOMRe.props foo::bar ()) [|child1, child2|];`

So why is the middle step even there? I suspect it's in order to decouple the JSX from Reason-React and its ReactDOMRe module. Not sure how you'd avoid the ppx in practice though... [TODO: explain better]


### High-order components and other patterns

## Appendix: Esoteric but useful resources

Much of the BuckleScript and Reason ecosystem is in a state of flux and is therefore only partially documented, or not documented at all! It can therefore be very useful to look at the source code, or interface files if available, to get a better idea of what's available and how it works. Uhmm, Good Luck!

* [ReasonJs source code](https://github.com/BuckleTypes/reason-js/blob/master/src/reasonJs.re)
* [ReactRe interface source](https://github.com/reasonml/reason-react/blob/master/src/reactRe.rei)
