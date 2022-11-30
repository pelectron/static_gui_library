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

As you can see it is quite easy. All that needs to be done is to translate the 
user input to an sgl::input value and call the menus handle_input() with that 
value. Tick updates are also very easy. If a tick needs to happen, for example 
indicated by a timer peripheral, simply call the menus tick() function. 
In the example, the tick is also used for the frame rate of the menu, i.e. 
every time the menu is ticked, it's content is printed on the display.

Note that both handle_input() and tick() should never be used in an ISR itself.
Always do it in this polling way in your main loop or another low priority task. 
These calls can become expensive, involving for example floating point formatting. 
Your gui updating exactly on the exact microsecond is not important, and running 
in an ISR is an easy way to blocking more important stuff from happening.

## Printing to a display

Printing to a display is quite easy. 
You can use the ``item_name()``, ``item_text()`` and ``page_name()`` methods of sgl::Menu.
These are simple to use and dont require any template programming to access the items content.

Another way is to use the ``sgl::for_current()`` or the ``for_each_page()`` method on the menu.
This can be useful for more specialized printing, i.e. when different pages or items need to 
be printed differently.

