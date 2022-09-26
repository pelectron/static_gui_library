# Item
Items are the things which occupy one line in the display. They can be used for displaying values, mutable and immutable text, or have more of a functional role (for example a button).
For a type `T` to satisfy the item concept, the following must hold:

variable      | type                                  |
--------------|---------------------------------------|
t             | T                                     |
ct            | const T                               |
i             | sgl::Input                            |
sv            | T::StringView                         |
m             | Menu containing t                     |
input_handler | [InputHandler](#input-handler) for T  |
tick_handler  | [TickHandler](#tick-handler) for T    |
click_handler | [ClickHandler](#click-handler) for T  |

For basic items, the following must hold:
Expression                           | Return type/value  | Notes                                   |
-------------------------------------|--------------------|-----------------------------------------|
`typename T::StringView`             | sgl::string_view   |                                         |
`typename T::String`                 | sgl::static_string |                                         |
`t.text()`                           | T::String&         |                                         |
`ct.text()`                          | const T::String&   |                                         |
`t.set_text(sv)`                     | sgl::error         |                                         | 
`t.clear_text()`                     | void               |                                         |
`t.handle_input(i)`                  | sgl::error         | calls the user provided input handler   |
`t.set_menu(&m)`                     | void               |                                         |
`t.tick()`                           | void               | calls the user provided tick handler    |
`t.set_input_handler(input_handler)` | T&                 |                                         |
`t.set_tick_handler(tick_handler)`   | T&                 |                                         |


Note that all of the operation listed above should be non throwing.