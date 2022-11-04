# Concepts

This library has a few concepts.
The concept is always provided as a table of variables and a table of expressions using those variables. The table of expressions must be fulfilled in order for the concept to be satisfied.

- [Item](#item)
- [Input Handler](#input-handler)
- [Click Handler](#click-handler)
- [Tick Handler](#tick-handler)
- [Formatter](#formatter)
- [Page Action](#page-action)

## Item

Items are the things which occupy one line in the display. They can be used for displaying values, mutable and immutable text, or have more of a functional role (for example a button).
For a type `T` to satisfy the item concept, the following must hold:

| variable | type       |
| -------- | ---------- |
| t        | T          |
| ct       | const T    |
| i        | sgl::Input |
| m        | sgl::Menu  |


| Expression              | type                                                   | Notes               |
| ----------------------- | ------------------------------------------------------ | ------------------- |
| `typename T::char_type` | `char / char16_t / char32_t`                           | typename            |
| `T::text_size`          | `size_t`                                               | constant expression |
| `t.handle_input(i)`     | `sgl::error`                                           | non throwing        |
| `t.set_menu(&m)`        | `void`                                                 | non throwing        |
| `t.tick()`              | `void`                                                 | non throwing        |
| `t.text()`              | `sgl::static_string<T::char_type,T::text_size>&`       |                     |
| `ct.text()`             | `const sgl::static_string<T::char_type,T::text_size>&` |                     |

sgl already provides various kinds of item types, but if it isn't enough, this is the starting point for creating your own.
All that is required are the three member functions described above. The ``handle_input`` method should return
sgl::error::edit_finished by default, as described in [input handling](input_handling.md).

This concept can be checked with ``sgl::is_item_v<ItemType>``.

The minimal working implementation of an item looks like this:

```cpp
#include <sgl.hpp>

class CustomItem{
public:
  using char_type = char;
  static constexpr size_t text_size = 25;

  sgl::error handle_input(sgl::Input) noexcept { return sgl::error::edit_finished; }

  void tick() noexcept {}

  void set_menu(void*) noexcept {}

  sgl::static_string<char_type, text_size>& text() noexcept { return str_; }

  const sgl::static_string<char_type, text_size>& text() const noexcept { return str_; }

private:
  sgl::static_string<char_type, text_size> str_;
};

static_assert(sgl::is_item_v<CustomItem>);
```

To reduced boilerplate and allow for the same mechanism of changing input and tick handlers as the item types sgl provides,
the custom type can inherit from [sgl::ItemBase](#sgl::ItemBase).

## Input Handler

An `InputHandler` handles user input for an item, page or menu. 
The syntactic requirements for a type F to be an `InputHandler` for T are the following:

| variable | type         |
| -------- | ------------ |
| `f`      | `F`          |
| `t`      | `T&`         |
| `i`      | `sgl::Input` |


| Expression                                  | Return type/value |
| ------------------------------------------- | ----------------- |
| `f(t, i)`                                   | sgl::error        |
| `noexcept(f(t, i))`                         | true              |
| `std::is_trivially_destructible_v<F>`       | true              |
| `std::is_trivially_move_constructible_v<F>` | true              |
| `std::is_trivially_copyable_v<F>`           | true              |

This syntactic requirements can be checked with `sgl::is_input_handler_for_v<F, T>`.

The few examples of input handlers:

```cpp
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

// Now for a type T provided by sgl, i.e. an item, sgl::Page or sgl::Menu
T t{/*...*/};

// any of the following lines will compile
t.set_input_handler(handler1);
t.set_input_handler(handler2);
t.set_input_handler(handler3);
t.set_input_handler(handler4);

sgl::Input i = /* ... */;
t.handle_input(i); // calls the handler
```

For more info on the whole input handling process, see [here](markdown/input_handling.md).


## Click Handler

A `ClickHandler` handles click inputs for a clickable [item](#item). It is called every time the item's click() method is called.
The following must hold for a type F to be an `ClickHandler` for Item.
| variable | type  |
| -------- | ----- |
| `f`      | F     |
| `item`   | Item& |


| Expression                                  | Return type or value |
| ------------------------------------------- | -------------------- |
| `f(item)`                                   | `sgl::error`         |
| `noexcept(f(item))`                         | `true`               |
| `std::is_trivially_destructible_v<F>`       | `true`               |
| `std::is_trivially_move_constructible_v<F>` | `true`               |
| `std::is_trivially_copyable_v<F>`           | `true`               |

This concept can be checked with `sgl::is_click_handler_for_v<F,Item>`.

## Tick Handler

A `TickHandler` handles tick events for an [item](#item). The tick handler is called when an item's tick() method is called. It is used to update an item through external means. 

The following must hold for a type F to be an `TickHandler` for Item.

| variable | type  |
| -------- | ----- |
| `f`      | F     |
| `item`   | Item& |


| Expression                                  | Return type or value |
| ------------------------------------------- | -------------------- |
| `f(item)`                                   | `void`               |
| `noexcept(f(item))`                         | `true`               |
| `std::is_trivially_destructible_v<F>`       | `true`               |
| `std::is_trivially_move_constructible_v<F>` | `true`               |
| `std::is_trivially_copyable_v<F>`           | `true`               |

This concept can be checked with `sgl::is_tick_handler_for_v<F,Item>`.

The few examples of tick handlers:

```cpp
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

## Formatter
Formatters are used by the sgl::Numeric class to automatically format the values into its text field.

For F to be a Formatter for Item, the following must hold:

| variable  | type             |
| --------- | ---------------- |
| f         | F                |
| str       | Item::char_type* |
| str_len   | size_t           |
| value     | Item::value_type |
| precision | uint32_t         |
| format    | sgl::format_t    |


| Expression                                            | Return type/value  |
| ----------------------------------------------------- | ------------------ |
| `f(str, str_len, value, precision, format)`           | sgl::format_result |
| `noexcept(f(str, str_len, value, precision, format))` | true               |
| `std::is_trivially_destructible_v<F>`                 | true               |
| `std::is_trivially_move_constructible_v<F>`           | true               |
| `std::is_trivially_copyable_v<F>`                     | true               |


## Page Action
 A page action is a callable that gets executed on certain events on a page. 
 For now, the only actions are the enter and exit actions, which are called when a page becomes the current page(i.e. is entered) or when the current page switches(i.e. is exited).

 For a type F to be a page action, the following must hold:
 
| variable | type       |
| -------- | ---------- |
| `f`      | F          |
| `page`   | sgl::Page& |


| Expression                                  | Return type or value |
| ------------------------------------------- | -------------------- |
| `f(page)`                                   | `sgl::error`         |
| `noexcept(f(page))`                         | `true`               |
| `std::is_trivially_destructible_v<F>`       | `true`               |
| `std::is_trivially_move_constructible_v<F>` | `true`               |
| `std::is_trivially_copyable_v<F>`           | `true`               |

An good example for an enter action would be a function that resets a pages current item index to 0 every time a page is entered, i.e. the page has no memory:

```cpp
template<typename Names, typename Types>
sgl::error reset_enter_action(sgl::Page<Names,Types>& page) noexcept{
  page.set_current_item(0);
  return sgl::error::no_error;
}
```

By default, this is not the case and a page will keep its current item index unchanged when being entered or exited.
