/*
 * Copyright (c) 2021, Andres Crucitti <dasc495@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "Board.h"
#include <AK/Random.h>
#include <time.h>

Board::Board(size_t columns, size_t rows)
    : m_columns(columns)
    , m_rows(rows)
{

    m_cells.resize(total_size());
    for (size_t i = 0; i < total_size(); ++i) {
        m_cells[i] = false;
    }
}

Board::~Board()
{
}

void Board::run_generation()
{
    m_stalled = true;
    Vector<bool> new_cells;
    new_cells.resize(total_size());

    for (size_t i = 0; i < total_size(); ++i) {
        bool old_val = m_cells[i];
        new_cells[i] = calculate_next_value(i);
        if (old_val != new_cells[i]) {
            m_stalled = false;
        }
    }

    if (m_stalled)
        return;

    m_cells = new_cells;
}

bool Board::calculate_next_value(int index)
{
    int row = index / columns();
    int column = index % columns();

    int top_left = get_cell(column - 1, row - 1) ? 1 : 0;
    int top_mid = get_cell(column, row - 1) ? 1 : 0;
    int top_right = get_cell(column + 1, row - 1) ? 1 : 0;
    int left = get_cell(column - 1, row) ? 1 : 0;
    int right = get_cell(column + 1, row) ? 1 : 0;
    int bottom_left = get_cell(column - 1, row + 1) ? 1 : 0;
    int bottom_mid = get_cell(column, row + 1) ? 1 : 0;
    int bottom_right = get_cell(column + 1, row + 1) ? 1 : 0;

    int sum = top_left + top_mid + top_right + left + right + bottom_left + bottom_mid + bottom_right;

    bool current = m_cells[index];
    bool new_value = current;

    if (current) {
        if (sum < 2 || sum > 3)
            new_value = false;
    } else {
        if (sum == 3)
            new_value = true;
    }

    return new_value;
}

void Board::clear()
{
    for (size_t i = 0; i < total_size(); ++i)
        set_cell(i, false);
}

void Board::randomize()
{
    for (size_t i = 0; i < total_size(); ++i)
        set_cell(i, get_random<u32>() % 2);
}
