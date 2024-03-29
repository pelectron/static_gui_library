# Integrating sgl

This section assumes you have read about

- [the architecture](architecture.md)
- [input handling](input_handling.md)
- [tick handling](external_updates.md)

The easiest way to show how to integrate the whole system is to use a simple example.

```cpp
#include <sgl.hpp>

// returns the user menu
constexpr auto get_menu()noexcept;
// outputs the menu on the user display. This function needs to be
void print_menu(const auto& menu)noexcept;

int main(){

  sgl::Menu m = get_menu();

  sgl::error ec { sgl::error::no_error };

  while(1){
    if(/* new user input*/){
      // received some user input from the embedded system, 
      /// for example from a keyboard or button. 
      // This input needs to be translated into an sgl::input
      sgl::input i = ...;

      // let the menu handle the user input
      ec = m.handle_input(i);


      if(ec != sgl::error::no_error){
        /*if error, handle error */
      }
    }
    if(/* tick needs to happen */){
      m.tick();
      // print again because state may have changed.
      print_menu(m);
    }

  }
}
```

As you can see it is quite easy. All that needs to be done is to translate the user input to an sgl::input value and call the menus handle_input() with that value.
Tick updates are also very easy. If a tick needs to happen, for example indicated by a timer peripheral, simply call the menus tick() function. In the example, the tick is also used for the frame rate of the menu, i.e. every time the menu is ticked, it's content is printed on the display.

Note that both handle_input() and tick() should never be used in an ISR itself. Always do it in this polling way in your main loop or another low priority task. These calls can become expensive, involving for example floating point formatting. Your gui updating exactly on the exact microsecond is not important, and running in an ISR is a quick way to blocking more important stuff from happening.

## Printing to a display
Printing to a display is completely up to you. sgl just provides a few convenience functions.
One way would be to use the page_name(), item_name() and item_text() functions of sgl::Menu. 
This will be the simplest in terms of implementation without requiring templates.

Another route is to use the for_current_page() and just ptrint the current page in the functor.
The page also provides item_name() and item_text() accessors, as well as 
for_each_item() / for_each_item_with_name.

Printing an item is also up to you. In the examples, item are formatted as "{item name}: {item text}".

