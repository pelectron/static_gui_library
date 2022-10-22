


# Concepts
This library has a few concepts.
The concept is always provided as a table of variables and a table of expressions using those variables. The table of expressions must be fulfilled in order for the concept to be satisfied.

# Item
Items are the things which occupy one line in the display. They can be used for displaying values, mutable and immutable text, or have more of a functional role (for example a button).
For a type `T` to satisfy the item concept, the following must hold:

variable | type                                  |
---------|---------------------------------------|
 t       | T                                     |
 i       | sgl::Input                            |
 m       | type satisfying the Menu concept      |

For basic items, the following must hold:
Expression          | Return type/value  | Notes        |
--------------------|--------------------|--------------|
`t.handle_input(i)` | sgl::error         | non throwing |
`t.set_menu(&m)`    | void               | non throwing |
`t.tick()`          | void               | non throwing |


# Input Handler
An `InputHandler` handles user input for an [item](item.md), [page](page.md) or [menu](menu.md). 
The syntactic requirements for a type F to be an `InputHandler` for T are the following:

variable  | type          |
----------|---------------|
`f`       | `F`           |
`t`       | `T&`          |
`i`       | `sgl::Input`  |


Expression                                  | Return type/value |
--------------------------------------------|-------------------|
`f(t, i)`                                   | sgl::error        |
`noexcept(f(t, i))`                         | true              |
`std::is_trivially_destructible_v<F>`       | true              |
`std::is_trivially_move_constructible_v<F>` | true              |
`std::is_trivially_copyable_v<F>`           | true              |

This syntactic requirements can be checked with `sgl::is_input_handler_for_v<F, T>`.

The few examples of input handlers:

```
// as a free function
sgl::error handler1(T& item, sgl::Input i) noexcept{
  // ...;
}

// as a lambda
auto handler2 = [](T& item, sgl::Input i) noexcept->sgl::error{
  // ...;
};

// an instance of a struct with call operator
struct Handler_t{
  sgl::error operator()(T& item, sgl::Input i)noexcept{
    // ...;
  }
};
Handler_t handler3 {/* ... */};

// items provided by sgl store their input handler as an sgl::Callable
sgl::Callable<sgl::error(T&,sgl::Input)> handler_4{/*...*/};

// Now for a type T provided by sgl, i.e. an Item, Page or Menu
T t{/*...*/};

// any of the following lines will compile
t.set_input_handler(handler1);
t.set_input_handler(handler2);
t.set_input_handler(handler3);
t.set_input_handler(handler4);

sgl::Input i = /* ... */;
t.handle_input(i); // calls the handler
```

# Click Handler
A `ClickHandler` handles click inputs for a clickable [item](#item). It is called every time the item's click() method is called.
The following must hold for a type F to be an `ClickHandler` for Item.
variable | type
---------|------
`f`      | F
`item`   | Item&


Expression                                  | Return type or value
--------------------------------------------|----------------------
`f(item)`                                   | `sgl::error`
`noexcept(f(item))`                         | `true`
`std::is_trivially_destructible_v<F>`       | `true`
`std::is_trivially_move_constructible_v<F>` | `true`
`std::is_trivially_copyable_v<F>`           | `true`

This concept can be checked with `sgl::is_click_handler_for_v<F,Item>`.

# Tick Handler
A `TickHandler` handles tick events for an [item](#item). The tick handler is called when an item's tick() method is called. It is used to update an item through external means. 

The following must hold for a type F to be an `TickHandler` for Item.

variable | type
---------|-----
`f`      | F
`item`   | Item&


Expression                                  | Return type or value
--------------------------------------------|----------------------
`f(item)`                                   | `void`
`noexcept(f(item))`                         | `true`
`std::is_trivially_destructible_v<F>`       | `true`
`std::is_trivially_move_constructible_v<F>` | `true`
`std::is_trivially_copyable_v<F>`           | `true`

This concept can be checked with `sgl::is_tick_handler_for_v<F,Item>`.

The few examples of tick handlers:

```
// as a free function
void handler1(Item& item) noexcept{
  // ...;
}

// as a lambda
auto handler2 = [](Item& item) noexcept -> void{
  // ...;
};

// an instance of a struct with call operator
struct Handler_t{
  sgl::error operator()(T& item, sgl::Input i)noexcept{
    // ...;
  }
};
Handler_t handler3 {/* ... */};

// items provided by sgl store their tick handler as an sgl::Callable
sgl::Callable<void(Item&)> handler_4{/* ... */};

// Now, for any T modelling Item, Page or Menu,
Item item{/*...*/};

// any of the following lines will compile
item.set_tick_handler(handler1);
item.set_tick_handler(handler2);
item.set_tick_handler(handler3);
item.set_tick_handler(handler4);

item.tick(); // calls the handler
```

# Formatter
Formatters are used by the sgl::Numeric class to automatically format the values into its text field.

For F to be a Formatter for Item, the following must hold:

variable  | type
----------|-----
f         | F
str       | Item::char_type*
str_len   | size_t
value     | Item::value_type
precision | uint32_t
format    | sgl::format_t


Expression                                            | Return type/value
------------------------------------------------------|--------------------
`f(str, str_len, value, precision, format)`           | sgl::format_result
`noexcept(f(str, str_len, value, precision, format))` | true
`std::is_trivially_destructible_v<F>`                 | true
`std::is_trivially_move_constructible_v<F>`           | true
`std::is_trivially_copyable_v<F>`                     | true
