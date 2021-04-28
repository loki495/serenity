#pragma once

#include <LibCore/Timer.h>
#include <LibGUI/Label.h>
#include <LibGUI/SpinBox.h>
#include <AK/Vector.h>
#include "Board.h"

class BoardWidget final : public GUI::Widget {
    C_OBJECT(BoardWidget);

public:

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void mousemove_event(GUI::MouseEvent&) override;
    virtual void mouseup_event(GUI::MouseEvent&) override;
    virtual void mousedown_event(GUI::MouseEvent& event) override;

    size_t get_index_for_point(int x, int y);
    void set_toggling_cells(bool toggling) { m_toggling_cells = toggling; if (!toggling) m_last_cell_toggled = m_board->total_size(); }
    size_t last_toggled() { return m_last_cell_toggled; }
    bool is_toggling() { return m_toggling_cells; }

    void toggle_cell(size_t index);
    void clear_cells() { m_board->clear(); }
    void randomize_cells() { m_board->randomize(); }

    int get_cell_size();

    void update_board(int columns, int rows);
    Board* board() const { return m_board; }

    bool is_running() const { return m_running; }
    void set_running(bool r);

    void set_toolbar_enabled(bool);

    void run_generation();

    int running_timer_interval() { return m_running_timer_interval; }
    void set_running_timer_interval(int interval);

    Function<void()> on_running_state_change;
    Function<void()> on_stall;
    Function<void(Board *, size_t)> on_cell_toggled;

private:
    BoardWidget(int columns, int rows);

    bool m_toggling_cells { false };
    size_t m_last_cell_toggled { 0 };

    Board* m_board { nullptr };

    bool m_running { false };

    int m_running_timer_interval { 500 };
    RefPtr<Core::Timer> m_timer;
};
