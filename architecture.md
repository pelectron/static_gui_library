# Architecture
Because of the nature of line based displays, sgl has the following visualization for the architecture:

Everything you see on a display at once are [items](item.md) in a [page](page.md).

Items are the things which occupy one line in the display.

The items on a page can be scrolled through, interacted with and optionally edited via user input.

One item, at any time, is the current item of a page.

An example display of (a part of) a page is below, where '->' marks the current item:

    _____________________
    |Configuration Page |
    |-> Settings        |
    |   some value: 45  |
    |   Version: 1.0.5  |
    |   Some other item |
    |   back            |
    |-------------------|

On a high level, a [page](page.md) can be thought of as simply an array of items with an index to keep track of the current item. Items of a page cannot be a page, i.e. pages cannot contain subpages.

Now, usually more than one page is needed for a menu.
In order to create a hierarchical structure to navigate, the pages need to be assembled into a [menu](menu.md).

On a high level, a menu can be thought of as simply an array of pages with an index to keep track of the current page.

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

In the example above, the current page of the menu is the 'Home Page', with 'item2' being the current item of 'Home Page' and as such the whole menu.
The other pages are not current, but they still have a current item, i.e. index.

And the resulting display output could look something like this:
    
    _____________
    | Home Page |
    |   item1   |
    | ->item2   |
    |   item3   |
    |   ...     |
    |-----------|

sgl provides various classes to support this structure.

The first is sgl::Menu. It implements a menu.

The second is sgl::Page. It implements a page.

The rest are item classes, which implement various items like:
 - [boolean items](#sgl::Boolean)
 - [numerical items](#sgl::Numeric)
 - [enumerated items](#sgl::Enum)
 - [buttons](#sgl::Button)

