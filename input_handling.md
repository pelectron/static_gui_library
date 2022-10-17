# Default Input Handling
This section explains how the default input handling works in sgl. It assumes you are familiar with the [architecture of sgl](architechture.md).

Like the architecture, input handling is split into three layers. 

## Menu Layer
A menu can handle user input with its handle_input() method. By default, a menu delegates the user input to the current page. To customize this behavior, set an [InputHandler](concepts.md/#input-handler) for the menu.

## Page Layer
The page input handling has two important jobs:
 - handle navigating through items on the page
 - handle editing and clicking of items

This library's way of doing this is as follows:

1. The page starts out in navigation mode. The keypad inputs Up, Down, Left, Right navigate
through the items. 
2. When an input equal to the page's start edit is received, the page switches
into edit mode and starts passing the input values on to the current item (including
the start edit value).

3. The returned value from the active item' handle_input() method is interpreted as such:
     - sgl::error:::edit_finished means the page should switch back into navigation mode,
     regardless of which input it just received. The page's default input handler will return
     sgl::error::no_error in this case. This is used for 'one and done' items like [buttons](sgl:Button) 
     and [links](sgl::PageLink). Else it would take significantly more complicated logic to handle these kinds of items.
     - sgl::error::no_error means the item handled the input successfully, and the page stays in
     edit mode. The page keeps relaying the inputs to the active item.The page's default input
     handler will return sgl::error::no_error in this case.
     - any other sgl::error value means the item failed to handle the input. The page switches
     back into navigation mode and returns the item's return value.

4. When an input equal to the page's stop edit is received, the page switches back into
navigation mode. The stop edit input is NOT passed on to the item.

As long as the convention of the item's input handler return value is held, the whole system
works as expected and you can mix and match item input handlers and page handlers, either library provided or your own.

Again, to change the page input handling, just provide an [InputHandler](concepts.md/#input-handler) for the page.

## Item Layer
The last layer is the item layer. Items receive user input from their containing page through the item's handle_input() method. The only requirement for the return value of the item's handle_input() method is that it respects the return value convention specified [above](#page-layer). That is:
 - sgl::error::no_error means no error.
 - sgl::error::edit_finished means the item itself decided it is done being edited.
 - any other value means an error occurred which you need to handle in your main loop.

If the default way of doing things is not to your liking, you can also create your own custom page input handler. Again, if the convention of the item's input handler return value is respected, the library provided item types work as expected.

## Putting it all together
All in all, the default input handling is quite simple:

- Menu's forward the input to their current page. 

- Pages process the input and decide wether the current item should process an input (edit mode) or if the page should be navigated (navigation mode).

- Items process the input received from their page. An item either processes the input successfully (sgl::error::no_error), successfully and wants to stop being edited (sgl::error::edit_finished) or with an error (any other sgl::error value).
