# Page
A page is a flat collection of [items](item.md). It is a non recursive, i.e. a page cannot contain a subpage. 
A page is responsible for navigating through the items, delegating user input the the current item, and handling the returned error of the current item.
The [default implementation](#sgl::Page) does this already.
For a type `T` to satisfy the page concept, the following must hold:

 | variable      | type                              |
 | ------------- | --------------------------------- |
 | t             | T                                 |
 | ct            | const T                           |
 | idx           | size_t                            |
 | i             | sgl::Input                        |
 | input_handler | InputHandler for T                |
 | tick_handler  | TickHandler for T                 |
 | name_vis      | NamedVisitor for items in T       |
 | cname_vis     | NamedVisitor for items in const T |
 | vis           | Visitor for T                     |
 | cvis          | Visitor for const T               |


| Expression                             | Return type/value            |
| -------------------------------------- | ---------------------------- |
| typename T::StringView                 | string view type of the page |
| ct.size()                              | size_t                       |
| ct.current_item_index()                | size_t                       |
| ct.is_in_edit_mode()                   | bool                         |
| ct.get_start_edit()                    | sgl::Input                   |
| ct.get_stop_edit()                     | sgl::Input                   |
| sgl::for_each(ct, cvis)                | void                         |
| sgl::for_each_with_name(ct, cname_vis) | void                         |
| sgl::for_current(ct, cvis)             | return type of cvis          |
| t.handle_input(i)                      | sgl::error                   |
| t.set_current_item(idx)                | T&                           |
| t.on_enter()                           | sgl::error                   |
| t.on_exit()                            | sgl::error                   |
| t.set_edit_mode()                      | void                         |
| t.set_navigation_mode()                | void                         |
| t.set_start_edit(i)                    | T&                           |
| t.set_stop_edit(i)                     | T&                           |
| t.set_menu(menu_ptr)                   | void                         |
| t.set_on_enter(action)                 | T&                           |
| t.set_on_exit(action)                  | T&                           |
| sgl::for_each_with_name(t, name_vis)   | void                         |
| sgl::for_current(t, vis)               | return type of vis           |


Note that all of the operation listed above should be non throwing.

This concept can be checked with `sgl::is_page_v<T>`.