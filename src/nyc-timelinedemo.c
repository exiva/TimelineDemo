#include <pebble.h>

static Window *window;
static MenuLayer *main_menu;
static int settings[4] = {0,0,0,0};

enum {
  CONFIG = 0,
  SUB = 1,
  STATUS = 2,
  GROUP = 3
};

static int16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 2;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_num_rows_callback(MenuLayer *menu_layer, int16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return 3;
    break;
    default:
      return 1;
  }
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, "Individual User Pins");
    break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "Subscription Pins");
    break;
  }
}


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GRect bounds = layer_get_bounds(cell_layer);

  if (cell_index->section == 0) {
    //Section 1. Basic demos.
    switch(cell_index->row) {
      case 0:
        menu_cell_basic_draw(ctx, cell_layer, "Insert Pin", NULL, NULL);
      break;
      case 1:
        menu_cell_basic_draw(ctx, cell_layer, "Update Pin", NULL, NULL);
      break;
      case 2:
        menu_cell_basic_draw(ctx, cell_layer, "Delete Pin", NULL, NULL);
      break;
    }
  } else {
    switch (cell_index->row) {
      case 0:
        menu_cell_basic_draw(ctx, cell_layer, "Word of Day", "Generic Layout", NULL);
      break;
    }

    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorWhite);

    if (menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
    }

    graphics_fill_rect(ctx, GRect(bounds.size.w - 20, (bounds.size.h / 2) - 7, 17, 17), 3, GCornersAll);
    graphics_draw_round_rect(ctx, GRect(bounds.size.w - 20, (bounds.size.h / 2) - 7, 17, 17), 3);

    if ((bool)settings[cell_index->row]) {
      graphics_context_set_fill_color(ctx, GColorBulgarianRose);
      graphics_fill_rect(ctx, GRect(bounds.size.w - 18, (bounds.size.h / 2) - 5, 13, 13), 2, GCornersAll);
    }
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "section %d row %d", cell_index->section, cell_index->row);
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, GROUP, &cell_index->section, sizeof(cell_index->section), true);
  dict_write_int(iter, SUB, &cell_index->row, sizeof(cell_index->row), true);
  if (cell_index->section == 1) {
    dict_write_int(iter, STATUS, &settings[cell_index->row], sizeof(settings[cell_index->row]), true);
  }
  app_message_outbox_send();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  if (!dict_find(iter, SUB)) return;
  int sub = dict_find(iter, SUB)->value->int32;
  if ((bool)settings[sub]) {
    settings[sub] = 0;
  } else {
    settings[sub] = 1;
  }
  menu_layer_reload_data(main_menu);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  main_menu = menu_layer_create(bounds);

  menu_layer_set_highlight_colors(main_menu, GColorRajah, GColorWhite);
  menu_layer_set_click_config_onto_window(main_menu, window);

  menu_layer_set_callbacks(main_menu, NULL, (MenuLayerCallbacks) {
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback)menu_get_num_rows_callback,
    .draw_row = (MenuLayerDrawRowCallback)menu_draw_row_callback,
    .select_click = (MenuLayerSelectCallback)menu_select_callback,
    .get_num_sections = (MenuLayerGetNumberOfSectionsCallback)menu_get_num_sections_callback,
    .draw_header = (MenuLayerDrawHeaderCallback)menu_draw_header_callback,
    .get_header_height = menu_get_header_height_callback,

  });

  layer_add_child(window_layer, menu_layer_get_layer(main_menu));
}

static void init(void) {
  window = window_create();

  if (persist_exists(CONFIG)) {
    persist_read_data(CONFIG, &settings, sizeof(settings));
  }

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
  });

  window_stack_push(window, true);
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(64, 64);
}

static void deinit(void) {
  persist_write_data(CONFIG, &settings, sizeof(settings));
  menu_layer_destroy(main_menu);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
