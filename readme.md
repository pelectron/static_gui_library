# About
I wanted to create a solution for making gui menus for fixed-width, line based displays.
There are three important points for me:
 - no dynamic allocations
 - declarative style for end users
 - customizable behavior where needed

I also wanted a pure software solution, i.e. no dependency on a certain display controller type, mcu etc. This way, it's up to the end user on how to actually print to a screen.

# Architecture
Because of the nature of line based displays, I have to following visualization for the architecture:

Everything you see on a display at once is a list of **items** in a **page**. This list of items can be scrolled through. One item, at any time, is the active/current item.
An example is below, the '->' marks the current item:

    --------Display--------
    Configuration Page
    -> Settings
       some value: 45 
       Version: 1.0.5
       Some other item
       back
    ----End of Display-----

A **menu** is comprised of one or multiple pages, where one page is active at any given time. The hierarchy of the pages is flat, i.e. each page is standalone and does not 'contain' any subpages in the programmatic sense. In the example above, the item with the text 'Settings' is not really a page, it merely links to the settings page. More on this @ref sgl::PageLink_t "here".

In the end, the menu structure should look something like below, where '->' marks the active page and item of the menu.

    Menu
    ->Home page
          item1
        ->item2
          item3
          ...
      Configuration Page
          Settings
          some value: 45 
          Version: 1.0.5
          Some other item
          back
          ...
      Settings Page
          ...

For these concepts, there are 3 kinds of classes in this library. 

The first one in sgl::Menu_t. It represents the menu and contains the pages. 

The second kind is sgl::Page_t. It represents a page and contains items. 

The last is a family of classes, the items, the base class of which is sgl::Item_t. They implement basic item behavior.

# Input handling and processing
Input handling is divided up into three stages. Below I will describe the default input handling process. However, keep in mind that all three stages can be completely customized.

## Menu Input Handling
First, the menu receives an Input through it's handle_input() member function. The menu relays this input to it's input handler. The default input handler does nothing more than delegate the input to the active page and return the page's result.

### Customizing Menu Input Handling
As with all types of handlers in this library, you can replace the default behavior. There is however not much to be done here, except cycle through all pages and delegate the input to the active one. And this is already implemented.

## Page Input Handling
The second stage is the page layer. This process is explained @ref page_input_handling "here" in detail.

## Item input Handling
The last stage is the item layer. Items also have an input handler, though the default one of Item_t just returns sgl::error::edit_finished. The item types provided by this library always have some default, hopefully sensible, behavior. It can however be completely customized. See the section @ref item_types "Item Types" for more info.
