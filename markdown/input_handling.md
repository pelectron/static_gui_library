# Input Handling

This section explains how the default input handling works in sgl. It assumes you are familiar with the [architecture of sgl](architecture.md).

Like the architecture, input handling is split into three layers.

## Menu Layer

A menu can handle user input with its handle_input() method. By default, a menu simply delegates the user input to the current page and returns the pages result. To customize this behavior, set an [InputHandler](concepts.md#input-handler) for the menu.

## Page Layer

The page layer has two important jobs:

- handle navigating through items on the page
- handle editing and clicking of items

This library's way of doing this is as follows:

1. The page starts out in navigation mode. The keypad inputs Up, Down, Left, Right navigate through the items of the current page.
2. When an input equal to the page's start edit is received, the page switches
into edit mode and starts passing the input values on to the current item (including
the start edit value).

3. The returned value from the active item' handle_input() method is interpreted as such:
     - sgl::error:::edit_finished means the page should switch back into navigation mode,
     regardless of which input it just received. The page's default input handler will return
     sgl::error::no_error in this case. This is used for 'one and done' items like [buttons](#sgl::Button) 
     and [links](#sgl::PageLink). Else it would take significantly more complicated logic to handle these kinds of items.
     - sgl::error::no_error means the item handled the input successfully, and the page stays in
     edit mode. The page keeps relaying the inputs to the active item.The page's default input handler will return sgl::error::no_error in this case.
     - any other sgl::error value means the item failed to handle the input. The page switches
     back into navigation mode and returns the error value.

4. When an input equal to the page's stop edit is received, the page switches back into
navigation mode. The stop edit input is NOT passed on to the item.

As long as the convention of the item's input handler return value is held, the whole system works as expected and you can mix and match item input handlers and page handlers, either library provided or your own.

Again, to change the page input handling strategy, simply provide an [InputHandler](concepts.md#input-handler) for the page.

## Item Layer

The last layer is the item layer. Items receive user input from their containing page through the item's handle_input() method. The only requirement for the return value of the method is that it respects the return value convention specified [above](input_Handling.md#page-layer). That is:

- sgl::error::no_error means no error.
- sgl::error::edit_finished means the item itself decided it is done being edited.
- any other value means an error occurred which the item cannot handle itself and an upper layer must decide what to do.

If the default way of doing things is not to your liking, you can also create your own custom [input handler](concepts.md#input-handler). Note that you really should respect the return value convention for correct interoperation with the rest of the library.

## Putting it all together

All in all, the default input handling is quite simple:

- Menu's forward the user input to their current page.

- Pages process the input and decide wether the current item should process an input (edit mode), if itself should be navigated (navigation mode), if it should ignore the input or if it should return an error.

- Active Items process the input received from their page. An item either processes the input successfully (sgl::error::no_error), successfully and wants to stop being edited (sgl::error::edit_finished) or with an error (any other sgl::error value, which by default also stops edit mode of the page).

In the end, the library user only needs to call the menus handle_input() method when a new input is received. This can be done in a low priority tasks or the main loop easily.

## sgl::Input

the enum class sgl::Input is used to handle all input in sgl. sgl assumes single character or action input. As such, Its underlying type is uint64_t to support the following requirements:

- accept all character type inputs, be it 8, 16 or 32 bits without modifying the input.
- accept more input types, like left, right up, down, enter.
- since a static cast conversion from custom integer constants is ok, a user of sgl can easily install his own masking on top of this library's masking. Then 56 lower bits are available.

The bits are split up as follows:
| Bit Nr  | Meaning                                                                     |
| ------- | --------------------------------------------------------------------------- |
| 63      | If 0-> keypad or custom input. If 1 -> character input.                     |
| 56 - 59 | reserved for keypad inputs.                                                 |
| 0 - 55  | free for use, but sgl assumes character inputs are at most 32 bits in size. |
