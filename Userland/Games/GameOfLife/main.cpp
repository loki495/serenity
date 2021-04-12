// TODO:
// - resize window on cell resize
// - mousemove over multiple cell

#include <LibGUI/Application.h>
#include <LibGUI/Button.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Menu.h>
#include <LibGUI/MenuBar.h>
#include <LibGUI/StatusBar.h>
#include <LibGUI/SpinBox.h>
#include <LibGUI/Toolbar.h>
#include <LibGUI/ToolbarContainer.h>
#include <LibGUI/Window.h>
#include <LibGUI/Slider.h>
#include <Games/GameOfLife/GameOfLifeGML.h>
#include <LibCore/ConfigFile.h>
#include "BoardWidget.h"

#define DBG \
    dbg() << "File: " << __FILE__ << " - Line: " << __LINE__

int main(int argc, char** argv) {

    auto config = Core::ConfigFile::get_for_app("GameOfLife");

    auto app = GUI::Application::construct(argc, argv);

    auto window = GUI::Window::construct();

    [[ maybe_unused ]] size_t board_cells_x = 20;
    [[ maybe_unused ]] size_t board_cells_y = 20;

    window->set_double_buffering_enabled(false);
    window->set_title("Game Of Life");

    auto& main_widget = window->set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(game_of_life_gml);
    main_widget.set_fill_with_background_color(true);

    auto& main_toolbar = *main_widget.find_descendant_of_type_named<GUI::Toolbar>("toolbar");

    RefPtr<GUI::Label> stalled_label = GUI::Label::construct();
    stalled_label->set_visible(false);
    stalled_label->set_text("Stalled...");
    stalled_label->set_text_alignment(Gfx::TextAlignment::CenterLeft);

    auto& board_widget_container = *main_widget.find_descendant_of_type_named<GUI::Widget>("board_widget_container");
    auto& board_layout = board_widget_container.set_layout<GUI::VerticalBoxLayout>();
    board_layout.set_spacing(0);
    auto& board_widget = board_widget_container.add<BoardWidget>(board_cells_x, board_cells_y);

    auto& statusbar = *main_widget.find_descendant_of_type_named<GUI::Statusbar>("statusbar");

    auto& columns_spinbox = *main_widget.find_descendant_of_type_named<GUI::SpinBox>("columns_spinbox");
    auto& rows_spinbox = *main_widget.find_descendant_of_type_named<GUI::SpinBox>("rows_spinbox");

    columns_spinbox.set_value(board_cells_x);
    rows_spinbox.set_value(board_cells_y);

    auto size_changed_function = [&] {
        board_widget.update_board(columns_spinbox.value(), rows_spinbox.value());
    };

    rows_spinbox.on_change = [&] (auto) { size_changed_function(); };
    columns_spinbox.on_change = [&] (auto) { size_changed_function(); };

    auto& interval_spinbox = *main_widget.find_descendant_of_type_named<GUI::SpinBox>("interval_spinbox");

    interval_spinbox.on_change = [&] (auto value) {
        board_widget.set_running_timer_interval(value);
    };

    interval_spinbox.set_value(150);

    auto interval_label = GUI::Label::construct();
    interval_label->set_fixed_width(15);
    interval_label->set_text("ms");

    main_toolbar.add_child(interval_label);

    auto paused_icon = Gfx::Bitmap::load_from_file("/res/icons/16x16/pause.png");
    auto play_icon = Gfx::Bitmap::load_from_file("/res/icons/16x16/play.png");

    auto toggle_running_action = GUI::Action::create("Toggle Running", { Mod_None, Key_Return }, *play_icon,  [&](GUI::Action&) {
        board_widget.set_running(!board_widget.is_running());
    });
    toggle_running_action->set_checkable(true);
    main_toolbar.add_action(toggle_running_action);

    auto run_one_generation_action = GUI::Action::create("Run Next Generation", { Mod_Ctrl, Key_Equal }, Gfx::Bitmap::load_from_file("/res/icons/16x16/go-forward.png"), [&](const GUI::Action&) {
        board_widget.run_generation();
    });
    main_toolbar.add_action(run_one_generation_action);

    auto clear_board_action = GUI::Action::create("Clear board", { Mod_Ctrl, Key_N},Gfx::Bitmap::load_from_file("/res/icons/16x16/delete.png"),  [&](auto&) {
        statusbar.set_text("Board Cleared...");
        board_widget.clear_cells();
        board_widget.update();
    });
    main_toolbar.add_action(clear_board_action);

    auto randomize_cells_action = GUI::Action::create("Randomize board", { Mod_Ctrl, Key_R }, Gfx::Bitmap::load_from_file("/res/icons/16x16/reload.png"), [&](auto&) {
        statusbar.set_text("Cells randomized...");
        board_widget.randomize_cells();
        board_widget.update();
    });
    main_toolbar.add_action(randomize_cells_action);

    auto menubar = GUI::Menubar::construct();
    auto& app_menu = menubar->add_menu("Game Of Life");

    app_menu.add_action(clear_board_action);
    app_menu.add_action(randomize_cells_action);
    app_menu.add_separator();
    app_menu.add_action(toggle_running_action);
    app_menu.add_action(run_one_generation_action);

    app_menu.add_action(GUI::CommonActions::make_quit_action([](auto&) {
        GUI::Application::the()->quit();
    }));

    auto& help_menu = menubar->add_menu("Help");
    help_menu.add_action(GUI::CommonActions::make_about_action("GameOfLife", app_icon, window));

    window->set_menubar(move(menubar));

    board_widget.on_running_state_change = [&] () {
        if (board_widget.is_running()) {
            statusbar.set_text("Running...");
            toggle_running_action->set_icon(play_icon);
            main_widget.set_override_cursor(Gfx::StandardCursor::None);
        } else {
            statusbar.set_text("");
            toggle_running_action->set_icon(paused_icon);
            main_widget.set_override_cursor(Gfx::StandardCursor::Drag);
        }

        interval_spinbox.set_value(board_widget.running_timer_interval());

        rows_spinbox.set_enabled(!board_widget.is_running());
        columns_spinbox.set_enabled(!board_widget.is_running());
        interval_spinbox.set_enabled(!board_widget.is_running());

        run_one_generation_action->set_enabled(!board_widget.is_running());
        clear_board_action->set_enabled(!board_widget.is_running());
        randomize_cells_action->set_enabled(!board_widget.is_running());

        board_widget.update();
    };

    board_widget.on_stall = [&] {
        toggle_running_action->activate();
        statusbar.set_text("Stalled...");
    };

    board_widget.on_cell_toggled = [&] (auto, auto) {
        statusbar.set_text("");
    };

    window->resize(400, 440);
    window->set_resize_aspect_ratio(1, 1);
    window->show();

    //window->set_icon(Gfx::Bitmap::load_from_file("/res/icons/16x16/.png"));

    return app->exec();
}
