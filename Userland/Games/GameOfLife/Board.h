/*
 * Copyright (c) 2021, Andres Crucitti <dasc495@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#include <AK/Vector.h>
#include <LibGfx/Point.h>
#include <stdio.h>

class Board {
public:
    Board(size_t columns, size_t rows);
    ~Board();

    size_t total_size() const { return m_columns * m_rows; }
    size_t columns() const { return m_columns; }
    size_t rows() const { return m_rows; }

    size_t calculate_index(int column, int row) const { return row * m_columns + column; };

    void toggle_cell(size_t index)
    {
        if (index > total_size() - 1)
            return;

        m_cells[index] = !m_cells[index];
    }

    void toggle_cell(int column, int row)
    {
        if (column < 0 || column > (int)total_size() - 1 || row < 0 || row > (int)total_size() - 1)
            return;

        size_t index = calculate_index(column, row);
        set_cell(index, !m_cells[index]);
    }

    void set_cell(int column, int row, bool on)
    {
        if (column < 0 || column > (int)total_size() - 1 || row < 0 || row > (int)total_size() - 1)
            return;

        size_t index = calculate_index(column, row);
        set_cell(index, on);
    }

    void set_cell(size_t index, bool on)
    {
        if (index > total_size() - 1)
            return;
        m_cells[index] = on;
    }

    bool get_cell(size_t index) const
    {
        if (index > total_size() - 1)
            return false;

        return m_cells[index];
    }

    bool get_cell(int column, int row) const
    {
        if (column < 0 || column > (int)total_size() - 1 || row < 0 || row > (int)total_size() - 1)
            return false;

        size_t index = calculate_index(column, row);
        return get_cell(index);
    }

    const Vector<bool>& cells() { return m_cells; }

    void run_generation();
    bool is_stalled() { return m_stalled; }

    Gfx::IntPoint index_to_col_row(size_t index)
    {
        if (index > total_size() - 1)
            return { -1, -1 };

        return { index % m_columns, index / m_columns };
    }

    void clear();
    void randomize();

private:
    bool calculate_next_value(int);

    size_t m_columns;
    size_t m_rows;

    bool m_stalled { false };

    Vector<bool> m_cells;
};
