# About
I wanted to create a solution for making gui menus for fixed-width, line based displays.
There are three important points for me:
 - no dynamic allocations
 - declarative style for end users
 - customizable behavior where needed
I also wanted a pure software solution, i.e. no dependency on a certain display controller type, mcu etc. This way, it's up to the end user on how to actually print on a device.

# Architecture
Because of the nature of line based displays, I have to following visualization for the architecture:

Everything you see on a display at once is a list of 'items' in a 'page'. This list of items can be scrolled through. One item, at any time, is the active/current item.
An example is below, the '<-' marks the current item:

--------Display--------
 - Page title
   - Settings <-
   - some value: 45 
   - Version: 1.0.5
   - back
   - ...

----End of Display-----

A page can of course contain more items in it's list than the physical display has lines.
If only one page was needed, that would be the whole menu. However, most projects require at least one subpage. In the example above, one would normally assume that hitting enter would lead to a settings page with entirely new items, and scrolling down to the 'back' item and hitting enter would go back to the 'super' page. 

As such, a 'menu' is comprised of one or multiple pages, where one page is active at any given time. The hierarchy of the pages is flat, i.e. each page is standalone and does not 'contain' any subpages. More on subpages in the page link section.

There are 3 kinds of classes in this library. 
The first one in sgl::Menu_t. It represents the menu and contains the pages. 
The second kind is sgl::Page_t. It represents a page and contains items. 
The last is a family of classes, the items, the base class of which is sgl::Item_t.