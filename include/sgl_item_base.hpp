#ifndef SGL_BASIC_ITEM_HPP
#define SGL_BASIC_ITEM_HPP
#include "sgl_callable.hpp"
#include "sgl_item_traits.hpp"
#include "sgl_static_string.hpp"
#include "sgl_string_view.hpp"

namespace sgl {

  /**
   * \brief
   * \tparam Traits
   */
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class ItemBase {
  public:
    static_assert(sgl::has_item_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'item_type'.");
    static_assert(sgl::has_char_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'char_type'.");
    static_assert(sgl::has_text_size_v<Traits>,
                  "Traits type needs a static constexpr member of type size_t called 'text_size'.");

    using traits_type = Traits;
    using item_type = ItemImpl;
    using char_type = typename traits_type::char_type;
    static constexpr size_t text_size = traits_type::text_size;
    static constexpr bool   clickable = sgl::get_clickable_v<traits_type>;
    using StringView = sgl::string_view<char_type>;
    using String = sgl::static_string<char_type, text_size>;

    /// concrete input handler type.
    using InputHandler_t = sgl::Callable<sgl::error(item_type&, sgl::Input)>;

    using TickHandler_t = sgl::Callable<void(item_type&)>;

    /**
     * \brief Construct an item with a name and text.
     * \param name name of the item
     * \param text text of the item
     */
    constexpr ItemBase(StringView name, StringView text) noexcept;

    /**
     * \brief Construct an item with it's name and text set to name_and_text.
     * \param name_and_text name and text of item.
     */
    constexpr ItemBase(StringView name_and_text) noexcept;

    /**
     * \brief Construct an item with name, text and input handler.
     * \tparam InputHandler input handler type
     * \param name name of the item
     * \param text text of the item
     * \param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr ItemBase(StringView name, StringView text, InputHandler&& handler) noexcept;

    template <typename InputHandler,
              typename TickHandler,
              enable_if_is_input_handler<InputHandler, item_type> = true,
              enable_if_is_tick_handler<TickHandler, item_type> = true>
    constexpr ItemBase(StringView name, StringView text, InputHandler&& input_handler, TickHandler&& tick_handler) noexcept;
    /**
     * \brief Construct an item with name, text and input handler.
     * \tparam InputHandler input handler type
     * \param name_and_text name and text of item
     * \param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr ItemBase(StringView name_and_text, InputHandler&& handler) noexcept;

    /**
     * \brief Can be used to give access to the menu from an item. Menu_t will
     * call this function for all items it contains with its declared type, i.e.
     * this function can be statically overridden in derived classes.See
     * basic_page_link for an example of how it is used.
     * \tparam Menu menu type
     */
    template <typename Menu>
    constexpr void set_menu(Menu*) noexcept;

    /**
     * \brief calls the items input handler.
     * \param input input
     * \return sgl::error::no_error in case of no error.
     * \return sgl::error::edit_finished in case the item is done being edited.
     * See input handling for more details.
     */
    constexpr sgl::error handle_input(sgl::Input input) noexcept;

    /**
     * \brief set the text field
     * \param new_text new text
     * \return sgl::error
     */
    constexpr sgl::error set_text(StringView new_text) noexcept;

    /**
     * \brief get const reference to text field
     * \return const String&
     */
    constexpr const String& get_text() const noexcept;

    /**
     * \brief get reference to text field
     * \return String&
     */
    constexpr String&    get_text() noexcept;
    constexpr StringView text() const noexcept { return text_; }
    /**
     * \brief clear the text field.
     */
    constexpr void clear_text() noexcept;

    /**
     * \brief get the name of the item
     * \return StringView
     */
    constexpr StringView name() const noexcept;

    /**
     * \brief Set the input handler.
     * \tparam InputHandler input handler type
     * \param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr void set_input_handler(InputHandler&& handler) noexcept;

    /**
     * @brief invoke tick handler
     */
    constexpr void tick() noexcept;

    template <typename TickHandler>
    constexpr void set_tick_handler(TickHandler&& handler) noexcept;

  private:
    static sgl::error default_handle_input(item_type& item, sgl::Input) noexcept;

    InputHandler_t handler_{&default_handle_input}; ///< handles user input
    TickHandler_t  tick_handler_{};                 ///< handles external update
    StringView     name_{};                         ///< name
    String         text_{};                         ///< text field
  };
} // namespace sgl
#include "impl/sgl_item_base_impl.hpp"
#endif