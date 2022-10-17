# About
I wanted to create a solution for making gui menus for fixed-width, line based displays.
There are three important points for me:
 - no dynamic allocations
 - declarative style for end users
 - customizable behavior where needed

I also wanted a pure software solution, i.e. no dependency on a certain display controller type, mcu etc. This way, it's up to the end user on how to actually print to a , with provided examples on how to do that.

# A small example to show the benefits
Below is a very basic example, showing how to create a menu.
\include example/example_readme.cpp

Also included is a simple command line tester.
\include example/menu_tester.cpp

# Architecture
Because of the nature of line based displays, I have the following visualization for the architecture:

Everything you see on a display at once is a list/array of **items** in a **page**. This list of items can be scrolled through. One item, at all times, is the active/current item of the page.
An example of (a part of) a page is below, where '->' marks the current item:

    --------Display--------
    Configuration Page
    -> Settings
       some value: 45 
       Version: 1.0.5
       Some other item
       back
    ----End of Display-----

**Items** are the things which occupy one line in the display. They can be used for displaying values, mutable and immutable text, or have more of a functional role (for example a button).

A **page** is (almost) nothing more than a container of items, with an additional index for keeping track of the current item. A page cannot have another page as an item. 

A **menu** is a container of pages, where one page is active at any given time. The hierarchy of the pages is flat because each page is standalone and does not 'contain' any subpages in the programmatic sense. In the example above, the item with the text 'Settings' is not a page, it merely links to a 'Settings Page'. More on this @ref sgl::PageLink "here".

In the end, the menu structure should look something like below, where '->' marks the current page and '-->' marks the current item of a page.

    Menu
    ->Home Page
           item1
        -->item2
           item3
          ...
      Configuration Page
           Settings
           some value: 45 
           Version: 1.0.5
        -->Some other item
           back
      Settings Page
          ...

In the example above, the current page of the menu is the 'Home Page', with 'item2' being the current item of 'Home Page' and the whole menu.
The other pages are not current, but they still have a current item, i.e. index.

## Classes
For these concepts, there are 3 kinds of class templates in this library. 

The first one is \ref sgl::Menu "sgl::Menu". It implements a menu. 

The second class is \RefPageType. It implements a page. 

The last is a family of classes, the items. They implement basic item behavior.
This library provide the following basic items:
 - \ref sgl::Button "boolean items"
 - \ref sgl::Numeric "numerical items"
 - \ref sgl::Enum "enumerated items"
 - \ref sgl::Button "buttons"
 - \RefPageType

# Input handling and processing
Input handling is divided up into three stages. Below I will describe the default input handling process. However, keep in mind that all three stages can be completely customized.

## Menu Input Handling
First, the menu receives an Input through it's handle_input() member function. The menu relays this input to it's input handler. The default input handler does nothing more than delegate the input to the active page and return the page's result.

### Customizing Menu Input Handling
As with all types of handlers in this library, you can replace the default behavior. There is however not much to be done here, except delegate the input to the active page, which this is already implemented.


## Page Input Handling
The second stage is the page layer. The page input handling has two important jobs:
 - handle navigating through items on the page
 - handle editing and clicking of items

The library way of doing this is as follows (read the documentation for sgl::Page to follow completely):
The page starts out in navigation mode. The keypad inputs Up, Down, Left, Right navigate through the items. 
When an input equal to the page's start edit is received, the page switches into edit mode and passes the input value on to the items input handler (including the start_edit value).
The returned value from the item input handler is interpreted as such:
 - sgl::error:::edit_finished means the page should switch back into navigation mode, regardless of which input it just received. The page's default input handler will return sgl::error::no_error in this case.
 - sgl::error::no_error means the item handled the input successfully, and the page stays in edit mode. The page keeps relaying the inputs to the active item.The page's default input handler will return sgl::error::no_error in this case.
 - any other sgl::error value means the item failed to handle the input. The page switches back into navigation mode and returns the error value from the item input handler.

When an input equal to the page's stop edit is received, the page switches back into navigation mode. The stop edit input is NOT passed on to the item input handler.

As long as the convention of the item's input handler return value is held, the whole system works as expected and you can mix and match item input handlers.

If the default way of doing things is not to your liking, you can also create your own custom page input handler. Again, if the convention of the item's input handler return value is respected, the library provided item types work as expected.



## Item input Handling
The last stage is the item layer. Items also have an input handler. An input handler for an item type 'Item' will always have the signature sgl::error(Item&,sgl::Input). The input handler will be called with a reference to the item it handles the input for and the user input. The return value should either be 
 - sgl::error::edit_finished, in case the item cannot be edited, the user input indicates that the editing of the item has finished, or the item has only a 'one input and done' mechanism like a button.
 - sgl::error::no_error, in case the provided input could be handled correctly and the item should remain in editing mode from it's point of view.
 - any other value indicates failure which cannot be handled by the item.
