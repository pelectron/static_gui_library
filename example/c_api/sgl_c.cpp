#include "sgl_c.h"
#include "sgl.hpp"

extern "C"{

void        sgl_init(void){}
int         sgl_handle_input(sglInput_t i){}
void        sgl_tick(void){}
size_t      sgl_current_page_size(void){}
const char* sgl_current_page_name(void){}
const char* sgl_item_name(size_t i){}
const char* sgl_item_text(size_t i){}
}
