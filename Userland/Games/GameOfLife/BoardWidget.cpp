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

int BoardWidget::get_cell_size() {
    [[ maybe_unused ]]float width = rect().width() / m_board->columns();
    [[ maybe_unused ]]float height = rect().height() / m_board->rows();

    int size = (int)width;

    if (rect().width() > rect().height()) {
        size = (int)height;
    }

    return size;
}

void BoardWidget::paint_event(GUI::PaintEvent& event)
{
    GUI::Widget::paint_event(event);

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());

    int cell_size = get_cell_size();

    printf("board: %dx%d - cell: %d\n",rect().width(), rect().height(), cell_size);

    for (size_t row = 0; row < m_board->rows(); ++row) {
        for (size_t column = 0; column < m_board->columns(); ++column) {
            int cell_x = column * cell_size;
            int cell_y = row * cell_size;

            Gfx::Rect cell_rect(cell_x, cell_y, cell_size, cell_size);

            Color border_color = Color::DarkGray;
            Color fill_color;

            bool on = m_board->get_cell(m_board->calculate_index(column, row));
            if (on) {
                fill_color = Color::from_rgb(Gfx::make_rgb(220,220,80));
            } else {
                fill_color = Color::MidGray;
            }

            painter.fill_rect(cell_rect, fill_color);
            painter.draw_rect(cell_rect, border_color);
        }
    }
}

void BoardWidget::mousedown_event(GUI::MouseEvent& event)
{
    size_t index = get_index_for_point(event.x(), event.y());
    set_toggling_cells(true);
    toggle_cell(index);
}

void BoardWidget::mousemove_event(GUI::MouseEvent& event)
{
    size_t index = get_index_for_point(event.x(), event.y());
    if (is_toggling()) {
        if (last_toggled() != index)
            toggle_cell(index);
    }
}

void BoardWidget::mouseup_event(GUI::MouseEvent&)
{
    set_toggling_cells(false);
}

size_t BoardWidget::get_index_for_point(int x, int y) {
    int cell_size = get_cell_size();
    return m_board->columns() * (y / cell_size) + x / cell_size;
}
