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

#include <getopt.h>
#include <ncurses.h>
#include <string>
#include "p4bisect.h"

#define COLOR_PAIR_SELECTED 1
#define COLOR_PAIR_HELP 2
#define COLOR_PAIR_MESSAGES 3

#define HELP_MESSAGE "g:good rev,b:bad rev,UP/DOWN/LEFT/RIGHT:cursor,q:quit,s:switch changes/labels"

#define LOGV(f, args...) fprintf(stdout, f, ## args)
#define LOGE(f, args...) fprintf(stderr, "Error: " f, ## args)

struct view
{
	WINDOW *window;
	unsigned int x, y, width, height;
};

struct view rev_view, help_view, msg_view;
P4Bisect *p4bisect;
static int screen_y, screen_x;
static unsigned long long rev_view_first, rev_view_cur;
static const struct option options[] =
{
	{ "file", 1, 0, 'f' },
	{ "good", 1, 0, 'g' },
	{ "bad", 1, 0, 'b' },
	{ "labels", 1, 0, 'l' },
	{ "changes", 1, 0, 'c' },
	{ "nosync", 1, 0, 'n' },
	{ "help", 0, 0, 'h' },
	{ NULL, 0, 0, 0 },
};
static const char *optstr = "f:g:b:lcnh";
int debug = 1;
static std::string messages;

static void usage(void)
{
	LOGV("Usage: p4bisect [options]\n");
	LOGV("Options:\n");
	LOGV("  -f, --file    //YOUR_DEPOT/YOUR_FILE\n");
	LOGV("  -g, --good    Specify the good revision\n");
	LOGV("  -b, --bad     Specify the bad revision\n");
	LOGV("  -l, --labels  Use P4 labels command\n");
	LOGV("  -c, --changes Use P4 changes command\n");
	LOGV("  -n, --nosync  Don't use P4 sync to sync code\n");
	LOGV("  -h, --help    Show help message and exit\n");
}

void init_display(void)
{
	initscr();

	// Enable keypad mode (allow use of the up and down arrows)
	keypad(stdscr, true);

	// Start using color
	start_color();
	use_default_colors();
	init_pair(COLOR_PAIR_SELECTED, COLOR_WHITE, COLOR_GREEN);
	init_pair(COLOR_PAIR_HELP, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_PAIR_MESSAGES, COLOR_RED, COLOR_BLACK);

	// Update the screen
	refresh();

	// Hide the cursor, set noecho
	curs_set(0);
	noecho();
	leaveok(stdscr, FALSE);
}

void init_rev_view(void)
{
	rev_view_cur = p4bisect->nr_revisions() / 2;
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
	wclear(help_view.window);
	wclear(msg_view.window);

	getmaxyx(stdscr, screen_y, screen_x);

	if ((rev_view_cur - rev_view_first) >= (unsigned long long)screen_y) {
		rev_view_first = rev_view_cur - screen_y / 2 / 2;
	}

	// Initialize windows
	help_view.y = screen_y - 2;
	help_view.x = rev_view.x;
	help_view.height = 1;
	help_view.width = screen_x;
	msg_view.y = screen_y - 1;
	msg_view.x = rev_view.x;
	msg_view.height = 1;
	msg_view.width = screen_x;
	rev_view.y = 0;
	rev_view.x = 0;
	rev_view.height = screen_y - help_view.height - msg_view.height;
	rev_view.width = screen_x;

	rev_view.window = newwin(rev_view.height, rev_view.width,
					rev_view.y, rev_view.x);
	msg_view.window = newwin(msg_view.height, msg_view.width,
					msg_view.y, msg_view.x);
	help_view.window = newwin(help_view.height, help_view.width,
					help_view.y, help_view.x);

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
	wattron(help_view.window, COLOR_PAIR(COLOR_PAIR_HELP) | A_BOLD);
	draw_text(help_view.window, 0, 0, HELP_MESSAGE, help_view.width);
	wattroff(help_view.window, COLOR_PAIR(COLOR_PAIR_HELP) | A_BOLD);

	wattron(msg_view.window, COLOR_PAIR(COLOR_PAIR_MESSAGES) | A_BOLD);
	draw_text(msg_view.window, 0, 0, messages.c_str(), msg_view.width);
	wattroff(msg_view.window, COLOR_PAIR(COLOR_PAIR_MESSAGES) | A_BOLD);

	// Update the screen
	refresh();
	wrefresh(rev_view.window);
	wrefresh(help_view.window);
	wrefresh(msg_view.window);
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

void draw_sync_msg(const char *msg)
{
	wattron(msg_view.window, COLOR_PAIR(COLOR_PAIR_MESSAGES) | A_BOLD);
	draw_text(msg_view.window, 0, 0, msg, msg_view.width);
	wattroff(msg_view.window, COLOR_PAIR(COLOR_PAIR_MESSAGES) | A_BOLD);

	wrefresh(msg_view.window);
}

int main(int argc, char **argv)
{
	int loptidx, c, use_changes = 0, no_sync = 0;
	const char *file = NULL, *good = NULL, *bad = NULL;
	int input = 0;

	for (;;) {
		c = getopt_long(argc, argv, optstr, options, &loptidx);
		if (c == -1)
			break;

		switch (c) {
		case 'f':
			file = optarg;
			break;

		case 'g':
			good = optarg;
			break;

		case 'b':
			bad = optarg;
			break;

		case 'c':
			use_changes = 1;
			break;

		case 'l':
			use_changes = 0;
			break;

		case 'n':
			no_sync = 1;
			break;

		case 'h':
			usage();
			return 0;

		default:
			LOGE("Unknown argument: %c\n", c);
			usage();
			return 0;
		}
	}

	if (!file || !good || !bad) {
		LOGE("file, good revision, bad revigion must be given\n");
		usage();
		return 0;
	}

	p4bisect = new P4Bisect();
	if (!p4bisect) {
		LOGE("Fail to create P4Bisect\n");
		return -1;
	}

	if (p4bisect->start(file, use_changes, good, bad)) {
		LOGE("Fail to start p4bisect\n");
		return -1;
	}

	init_rev_view();

	init_display();

	draw_windows();

	if (!no_sync) {
		p4bisect->SyncRevision(rev_view_cur, draw_sync_msg);
	}

	while ((char(input) != 'q') && (char(input) != 'Q')) {
		draw_windows();
		messages = "";

		input = getch();
		switch (input) {
		case KEY_UP:
			rev_view_steps(-1);
			break;

		case KEY_DOWN:
			rev_view_steps(1);
			break;

		case KEY_LEFT:
			rev_view_steps(0 - rev_view.width / 2);
			break;

		case KEY_RIGHT:
			rev_view_steps(rev_view.width / 2);
			break;

		case 'g':
		case 'G':
			if (p4bisect->MarkRevision(rev_view_cur, 1)) {
				messages = "Cannot mark it as a good revision";
			} else {
				rev_view_cur = p4bisect->CurrentRevision();
				if (!no_sync) {
					p4bisect->SyncRevision(rev_view_cur,
							draw_sync_msg);
				}
			}
			break;

		case 'b':
		case 'B':
			if (p4bisect->MarkRevision(rev_view_cur, 0)) {
				messages = "Cannot mark it as a bad revision";
			} else {
				rev_view_cur = p4bisect->CurrentRevision();
				if (!no_sync) {
					p4bisect->SyncRevision(rev_view_cur, 
							draw_sync_msg);
				}
			}
			break;

		case 's':
		case 'S':
			use_changes ^= 1;
			if (p4bisect->start(file, use_changes, good, bad)) {
				messages = "Fail to switch";
			}
			rev_view_cur = p4bisect->CurrentRevision();
			if (!no_sync) {
				p4bisect->SyncRevision(rev_view_cur, 
						draw_sync_msg);
			}
			break;

		default:
			break;
		}
	}

	endwin();

	return 0;
}
