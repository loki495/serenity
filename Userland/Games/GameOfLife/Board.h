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

    void toggle_cell(size_t index);
    void toggle_cell(int column, int row);

    void set_cell(int column, int row, bool on);
    void set_cell(size_t index, bool on);

    bool get_cell(int column, int row) const;
    bool get_cell(size_t index) const;

    const Vector<bool>& cells() { return m_cells; }

    void run_generation();
    bool is_stalled() { return m_stalled; }

    void clear();
    void randomize();

private:
    bool calculate_next_value(int);

    size_t m_columns;
    size_t m_rows;

    bool m_stalled { false };

    Vector<bool> m_cells;
};
