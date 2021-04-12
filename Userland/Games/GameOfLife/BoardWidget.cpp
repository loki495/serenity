#include <LibGfx/Bitmap.h>
#include <LibGfx/Rect.h>
#include <LibGUI/Action.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Label.h>
#include <LibGUI/Painter.h>
#include <LibGUI/SpinBox.h>
#include "BoardWidget.h"

BoardWidget::BoardWidget(int columns, int rows)
{
    auto &main_layout = set_layout<GUI::VerticalBoxLayout>();
    main_layout.set_spacing(0);

    m_timer = add<Core::Timer>();
    m_timer->stop();
    m_timer->on_timeout = [this] {
        run_generation();
    };
    m_timer->set_interval(m_running_timer_interval);

    update_board(columns, rows);
}

void BoardWidget::run_generation() {
    m_board->run_generation();
    update();
    if (m_board->is_stalled()) {
        if (on_stall)
            on_stall();
        update();
    };
}

void BoardWidget::update_board(int columns, int rows)
{
    set_running(false);

    m_last_cell_toggled = columns * rows;

    if (m_board) {
        if (columns == (int)m_board->columns() && rows == (int)m_board->rows()) {
            return;
        }
    }

    delete m_board;
    m_board = new Board(columns, rows);

    while(child_widgets().size()) {
        auto container = child_widgets().first();
        while(container->child_widgets().size()) {
            auto cell = container->child_widgets().first();
            cell->remove_from_parent();
        };
        container->remove_from_parent();
    };

    for (size_t row = 0; row < m_board->rows(); ++row) {
        auto& container = add<GUI::Widget>();
        auto& horizontal_layout = container.set_layout<GUI::HorizontalBoxLayout>();
        horizontal_layout.set_spacing(0);
        for (size_t column = 0; column < m_board->columns(); ++column) {
            auto cell = Cell::construct(this, m_board->calculate_index(column,row));
            container.add_child(cell);
        }
    }

    update();
}
void BoardWidget::set_running_timer_interval(int interval) {
    if (is_running())
        return;

    m_running_timer_interval = interval;
    m_timer->set_interval(m_running_timer_interval);

    if (on_running_state_change)
        on_running_state_change();
}

void BoardWidget::set_running(bool r) {
    if (r == m_running)
        return;

    m_running = r;

    if (m_running) {
        m_timer->start();
    } else {
        m_timer->stop();
    }

    if (on_running_state_change)
         on_running_state_change();

    update();
}

void BoardWidget::toggle_cell(size_t index) {
    if (m_running || !m_toggling_cells || m_last_cell_toggled == index)
        return;

    m_last_cell_toggled = index;
    m_board->toggle_cell(index);

    if (on_cell_toggled)
        on_cell_toggled(m_board, index);

    update();
}

BoardWidget::Cell::Cell(BoardWidget* board, size_t index)
    : GUI::Label()
    , m_board_widget(board)
    , m_index(index)
{
    set_greedy_for_hits(false);
}

void BoardWidget::Cell::paint_event(GUI::PaintEvent& event)
{
    Frame::paint_event(event);

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());

    Color border_color = Color::DarkGray;
    Color fill_color;

    bool on = m_board_widget->board()->get_cell(m_index);
    if (on) {
        fill_color = Color::from_rgb(Gfx::make_rgb(220,220,80));
    } else {
        fill_color = Color::MidGray;
    }

    painter.fill_rect(rect(), fill_color);
    painter.draw_rect(rect(), border_color);
}

void BoardWidget::Cell::mousedown_event(GUI::MouseEvent&) {
    m_board_widget->set_toggling_cells(true);
    m_board_widget->toggle_cell(m_index);
}
void BoardWidget::Cell::mousemove_event(GUI::MouseEvent&)  {
    if (m_board_widget->is_toggling()) {
        if (m_board_widget->last_toggled() != m_index)
            m_board_widget->toggle_cell(m_index);
    }
}
void BoardWidget::Cell::mouseup_event(GUI::MouseEvent&) {
    m_board_widget->set_toggling_cells(false);
}
