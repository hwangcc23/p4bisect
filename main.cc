/*
 * main.cc
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <ncurses.h>
#include "p4bisect.h"

#define COLOR_PAIR_SELECTED 1
#define COLOR_PAIR_STATUS 2

struct view
{
	WINDOW *window;
	unsigned int x, y, width, height;
};

struct view rev_view, stat_view;
P4Bisect *p4bisect;
static int screen_y, screen_x;
static unsigned long long rev_view_first, rev_view_cur;

void init_display(void)
{
	initscr();

	// Enable keypad mode (allow use of the up and down arrows)
	keypad(stdscr, true);

	// Start using color
	start_color();
	use_default_colors();
	init_pair(COLOR_PAIR_SELECTED, COLOR_WHITE, COLOR_GREEN);
	init_pair(COLOR_PAIR_STATUS, COLOR_WHITE, COLOR_BLUE);

	// Update the screen
	refresh();

	// Hide the cursor, set noecho
	curs_set(0);
	noecho();
	leaveok(stdscr, FALSE);
}

void init_rev_view(void)
{
	unsigned long long offset;

	getmaxyx(stdscr, screen_y, screen_x);

	offset = screen_y / 2 / 2;
	rev_view_cur = p4bisect->nr_revisions() / 2;
	if (rev_view_cur <= offset) {
		rev_view_first = 0;
	} else {
		rev_view_first = rev_view_cur - offset;
	}
}

void draw_text(WINDOW *window, int y, int x, const char *str, int screen_width)
{
	int i;

	for (i = 0; i < (int)strlen(str) && i < screen_width; i++) {
		mvwprintw(window, y, x + i, "%c", str[i]);
	}
	for (; i < screen_width; i++) {
		mvwprintw(window, y, x + i, " ");
	}
}

void draw_windows(void)
{
	unsigned long long rev;
	unsigned int y;

	wclear(rev_view.window);
	wclear(stat_view.window);

	getmaxyx(stdscr, screen_y, screen_x);

	// Initialize windows
	rev_view.y = 0;
	rev_view.x = 0;
	rev_view.height = screen_y * 0.95;
	rev_view.width = screen_x;
	stat_view.y = rev_view.y + rev_view.height;
	stat_view.x = rev_view.x;
	stat_view.height = screen_y - rev_view.height;
	stat_view.width = screen_x;

	rev_view.window = newwin(rev_view.height, rev_view.width,
					rev_view.y, rev_view.x);
	stat_view.window = newwin(stat_view.height, stat_view.width,
					stat_view.y, stat_view.x);

	for (rev = rev_view_first, y = rev_view.y; 
			rev < p4bisect->nr_revisions(); 
			rev++, y++) {
		if (rev == rev_view_cur) {
			wattron(rev_view.window, 
					COLOR_PAIR(COLOR_PAIR_SELECTED) | A_BOLD);
			draw_text(rev_view.window, y, rev_view.x, 
					p4bisect->revision(rev), 
					rev_view.width);
			wattroff(rev_view.window, 
					COLOR_PAIR(COLOR_PAIR_SELECTED) | A_BOLD);
		} else {
			draw_text(rev_view.window, y, rev_view.x, 
					p4bisect->revision(rev), 
					rev_view.width);
		}
		if (y >= rev_view.height) {
			break;
		}
	}
	wattron(stat_view.window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
	draw_text(stat_view.window, 0, 0, "status", stat_view.width);
	wattroff(stat_view.window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);

	// Update the screen
	refresh();
	wrefresh(rev_view.window);
	wrefresh(stat_view.window);
}

void rev_view_steps(int steps)
{
	int i;

	if (steps < 0) {
		for (i = steps; i < 0; i++) {
			if (rev_view_cur == 0 && rev_view_first == 0) {
				break;
			}
			if (rev_view_cur == rev_view_first) {
				rev_view_first--;
			}
			if (rev_view_cur != 0) {
				rev_view_cur--;
			}
		}
	} else if (steps > 0) {
		for (i = 0; i < steps; i++) {
			if ((rev_view_cur + 1) == p4bisect->nr_revisions()) {
				break;
			}
			if (rev_view_cur <= p4bisect->nr_revisions()) {
				rev_view_cur++;
			}
			if (rev_view_cur == (rev_view_first + rev_view.height)) {
				rev_view_first++;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int input = 0;

	p4bisect = new P4Bisect();
	if (!p4bisect) {
		return -1;
	}

	init_display();

	init_rev_view();

	while ((char(input) != 'q') && (char(input) != 'Q')) {
		draw_windows();

		input = getch();
		switch (input) {
		case KEY_UP:
			rev_view_steps(-1);
			break;

		case KEY_DOWN:
			rev_view_steps(1);
			break;

		default:
			break;
		}
	}

	endwin();

	return 0;
}
