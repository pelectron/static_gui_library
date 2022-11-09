# External Updates

Most often, items need some way of being updated periodically and access global state.
To make it as easy and simple as possible, items must have a tick method.
This method gets called every time the menus tick method is called. This way, one
function call on the menu updates all items, which can easily be done in the main
loop or low priority task of your embedded System. Note that the menus tick can be
expensive depending the work done in the items ticks, for example floating point
formatting. Make sure to never call the menus tick in an ISR or time-critical section.

Now, requiring a new type for every behavior change would be silly, so the items
provided by sgl allow the user to install a [tick handler](concepts.md#tick-handler)
on the item. This tick handler is then called whenever the items tick method is called.

To make everything more clear, lets look at an example:

Suppose the task is to monitor a value coming from an 16 bit ADC peripheral and
the ADC buffer is only one sample long. Given is also the following:

- The buffer is accessible globally with the name `adc_buf` and of type `uint16_t*`.
- The character type is `char`.
- The text size is 25.
- The items type is `sgl::Numeric<25, char, uint16_t>`, i.e. a numeric item
  holding a uint16_t value,

One would write a tick handler as such:

```cpp
void adc_tick(sgl::Numeric<25, char, uint16_t>& adc_item){
  adc_item.set_value(*adc_buf);
}
```

When creating the item, the adc_tick method can simply be 'installed' with the
set_tick_handler() method:

```cpp
constexpr auto adc_item() noexcept{
  return sgl::Numeric<25, char, uint16_t>{0, 0}.set_tick_handler(&adc_tick);
}
```

An even compacter, and equivalent, method would be to use a non capturing lambda:

```cpp
constexpr auto adc_item() noexcept{

  return sgl::Numeric<25, char, uint16_t>{0, 0}.set_tick_handler([](auto& adc_item){
                                                                  adc_item.set_value(*adc_buf);
                                                                });
}
```

Note that the set_tick_handler() method returns `*this`, so direct return is possible.

Then, all that is needed to update the adc item is to call the menus tick method.
